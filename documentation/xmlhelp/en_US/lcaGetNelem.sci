function lcaGetNelem
//   Retrieve the element count of a number of PVs
//  Calling Sequence
//numberOfElements = lcaGetNelem(pvs)
//  Description
//   Retrieve the element count of a number of PVs. 
//   <note>
//   The element count is not
//   necessarily the number of valid elements (e.g. the actual number of
//   values read from a device into a waveform) but the maximum number of
//   elements a PV can hold.
//   </note>
//  Parameters
//   pvs: Column vector (in matlab: m x 1 cell- matrix) of m strings.
//   numberOfElements: m x 1 column vector of the PV's number of elements (``array dimension'').
endfunction
