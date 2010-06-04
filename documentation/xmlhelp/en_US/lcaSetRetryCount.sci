function lcaSetRetryCount(n)
//Set number retries when pending for data or events
//  Calling Sequence
//lcaSetRetryCount(newRetryCount)
//  Description
//   Set the ezca library retryCount parameter (consult the ezca
//   documentation for more information). The retry count multiplied by the
//   <link linkend='lcaSetTimeout'>timeout</link> parameter determines the
//   maximum time the interface waits
//   for connections and data transfers, respectively.
//See also
//   lcaGetRetryCount
//   lcaGetTimeout
//   lcaSetTimeout
endfunction
