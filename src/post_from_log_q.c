#include <stdlib.h>
#include <curl/curl.h>
#include "ab_constants.h"
#include "macros.h"
#include "ab_types.h"
#include "auxil.h"
#include "get_variant.h"
#include "post_from_log_q.h"
#include "zero.h"
#include "make_curl_payload.h"

#include "ab_globals.h"

void *
post_from_log_q(
    void *arg
    )
{
  CURLcode curl_res; 
  long http_code;
  PAYLOAD_TYPE lcl_payload;

  for ( ; ; ) {
    memset(&lcl_payload, '\0', sizeof(PAYLOAD_TYPE));
    pthread_mutex_lock(&g_mutex);	/* protect buffer */
    if ( (g_halt == true) && ( g_n_log_q == 0 ) ) {
      pthread_mutex_unlock(&g_mutex);	/* release the buffer */
      break; // get out of the loop and out of here
    }
    while ( (g_halt == false) && ( g_n_log_q == 0 ) ) {
      /* If there is nothing in the buffer then wait */
     //  fprintf(stderr, "consumer waiting\n");
      pthread_cond_wait(&g_condc, &g_mutex);
      // fprintf(stderr, "consumer still waiting\n");
    }
    if ( (g_halt == true) && ( g_n_log_q == 0 ) ) {
      pthread_mutex_unlock(&g_mutex);	/* release the buffer */
      break; // get out of the loop and out of here
    }
    // fprintf(stderr, "consumer read %d\n", buf[ridx]);
    int eff_rd_idx = g_q_rd_idx % g_sz_log_q;
    lcl_payload = g_log_q[eff_rd_idx];
    memset(&(g_log_q[eff_rd_idx]), '\0', sizeof(PAYLOAD_TYPE));
    g_q_rd_idx++; 
    g_n_log_q--;
    pthread_cond_signal(&g_condp);	/* wake up consumer */
    pthread_mutex_unlock(&g_mutex);	/* release the buffer */
    // Now that you are out of the critical section, do the POST
    if ( g_ch == NULL ) { /* Nothing to do */ continue; }
    // Now, here is the real work of this consumer - the POST
    memset(g_curl_payload, '\0', AB_MAX_LEN_PAYLOAD+1);
    make_curl_payload(lcl_payload, g_curl_payload);
    curl_easy_setopt(g_ch, CURLOPT_POSTFIELDS, g_curl_payload);
    g_log_posts++;
    int retry_count = 0;
    bool post_succeeded = false;
    for ( ;  retry_count < g_num_post_retries ; retry_count++ ) {
      curl_res = curl_easy_perform(g_ch);
      if ( curl_res != CURLE_OK ) { 
        g_log_failed_posts++;
        fprintf(stderr, "ERROR! POST Failed = %s\n", g_curl_payload);
        WHEREAMI;
        continue;
      }
      curl_easy_getinfo(g_ch, CURLINFO_RESPONSE_CODE, &http_code);
      if ( http_code != 200 )  { 
        g_log_failed_posts++;
        fprintf(stderr, "ERROR! POST Failed = %s\n", g_curl_payload);
        WHEREAMI;
        continue;
      }
      // If control comes here, it means we succeeded
      post_succeeded = true;
      break;
      sleep(1); // give logger some breathing room
    }
    if ( !post_succeeded ) { 
      g_log_bad_posts++;
      fprintf(stderr, "POST totally failed\n");  
    }
  }
  pthread_exit(NULL); 
  return NULL;
}