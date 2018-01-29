#include "ab_incs.h"
#include "auxil.h"
#include "ab_globals.h"
#include "get_body.h"

int 
get_body(
    AB_REQ_TYPE req_type,
    struct evhttp_request *req,
    char body[AB_MAX_LEN_BODY+1],
    int n_body
    )
{
  int status = 0;
  struct evbuffer *inbuf = NULL;
  char *cbuf = NULL;

  if ( ( req_type == GetVariant ) ||  ( req_type == GetVariants ) ) {
    return status;
  }
  memset(body, '\0', n_body+1);
  inbuf = evhttp_request_get_input_buffer(req);
  // TODO Why do we need while loop?
  while (evbuffer_get_length(inbuf)) {
    int n;
    cbuf = malloc(n_body+1);
    return_if_malloc_failed(cbuf);
    memset(cbuf, '\0', n_body+1);
    n = evbuffer_remove(inbuf, cbuf, sizeof(cbuf));
    if ( n > 0) {
      // verify that it is good JSON
    }
    else {
      go_BYE(-1);
    }
    free_if_non_null(cbuf);
  }
BYE:
  free_if_non_null(cbuf);
  return status;
}