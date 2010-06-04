function [n]=lcaGetRetryCount()
//Get number of retries when pending for data or events
//  Calling Sequence
//currentRetryCount = lcaGetRetryCount()
//  Description
//   Retrieve the ezca library retryCount parameter (consult the ezca
//   documentation for more information). The retry count multiplied by the
//   <link linkend='lcaGetTimeout'>timeout</link> parameter determines the
//   maximum time the interface waits
//   for connections and data transfers, respectively.
//See also
//   lcaGetTimeout
//   lcaSetRetryCount
//   lcaSetTimeout
endfunction
