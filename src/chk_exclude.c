#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ab_constants.h"
#include "macros.h"
#include "ab_types.h"
#include "auxil.h"
#include "chk_exclude.h"
#include "ab_globals.h"
#include "dump_log.h"

int
chk_exclude(
    int test_idx,
    const char *args,
    const char *uuid,
    bool *ptr_is_exclude
    )
{
  int status = 0;
  bool is_null_data = false;
  bool is_bad_data = false;
  char url[AB_MAX_LEN_URL+1];
  CURLcode curl_res;  long http_code = 0;
  uint64_t t_start = 0, t_stop = 0;
  *ptr_is_exclude = false;  // Default: do not exclude this UUID 
  if ( !g_tests[test_idx].has_filters ) { goto BYE; }
  //----------------------------------------------
  uint32_t x_tst_id = g_tests[test_idx].x_tst_id;
  // start assembling URL for session server
  memset(url, '\0', AB_MAX_LEN_URL+1);
  /* Sample 
   *  curl --url "logger/session/v1/cbcace73?caller_client_id=foo
   *  */
  size_t nw = snprintf(url, AB_MAX_LEN_URL,
      "%s:%d/%s/v1/%s/?caller_client_id=abrts&fields=", 
      g_ss_server, g_ss_port, g_ss_url, uuid);
  if ( nw > AB_MAX_LEN_URL ) { go_BYE(-1); } // Log this failure

  // Fire URL to session service
  // fprintf(stderr, "url = %s \n", url);
  t_start = RDTSC();
  g_log_ss_calls++;
  curl_easy_setopt(g_ss_ch, CURLOPT_URL, url);
  double ss_resp_time = 0;
  curl_res = curl_easy_perform(g_ss_ch);
  t_stop = RDTSC();

  if ( curl_res != CURLE_OK ) { go_BYE(-2); }
  curl_easy_getinfo(g_ss_ch, CURLINFO_RESPONSE_CODE, &http_code);
  if ( http_code != 200 )  {  
    fprintf(stderr, "url = %s, uuid = %s \n", url, uuid);
    g_log_ss_bad_code++;
    go_BYE(-2); 
  }
  for ( char *cptr = g_ss_response; *cptr != '\0'; cptr++ ) { 
    if ( !isascii(*cptr) ) { g_log_ss_non_ascii++; go_BYE(-1); }
  }
  strtolower(g_ss_response);
  curl_easy_getinfo(g_ss_ch, CURLINFO_TOTAL_TIME, &ss_resp_time);
  // log response time 
  // TODO Call Indrajeet
  bool match = false;
  if ( match ) { 
    *ptr_is_exclude = true; 
  } 
  else { 
    *ptr_is_exclude = false; 
  }

BYE:
  log_ss_calls(uuid, t_stop - t_start, http_code, 
      is_null_data, is_bad_data, g_ss_response);
  memset(g_ss_response, '\0', g_sz_ss_response);
  return status;
}