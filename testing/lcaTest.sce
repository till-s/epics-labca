// Test timeout and retry count

if ( 0 )
try

// argument check
if ( labcaversion > 2 )
	try
		lcaSetTimeout(0)
	catch
		if ( lcaLastError() ~= 1 )
			errxxx=lasterror()
			error(errxxx)
		end
	end
	try
		lcaSetRetryCount(0)
	catch
		if ( lcaLastError() ~= 1 )
			errxxx=lasterror()
			error(errxxx)
		end
	end
end

lcaSetTimeout(0.5)

if ( lcaGetTimeout() ~= 0.5 )
	error('Readback of timeout failed')
end

lcaSetRetryCount(4)

if ( lcaGetRetryCount() ~= 4 )
	error('Readback of retry count failed')
end

// estimate timeout
tic()
	try
	disp('Waiting for timeout to expire...')
    disp('An error >could not find process variable : xxxx5789z< is normal here')
	lcaGet('xxxx5789z')
	catch
		if ( lcaLastError() ~= 6 ) 
			disp('lcaGet from unkown PV timeout expected but got other error')
			errxxx=lasterror()
			error(errxxx)
		end
	end
tme = toc();
if ( tme < 1 | tme > 3 )
	error('timeout out of bounds')
end

catch
	disp('Timeout/Retry count test FAILED')
	errxxx=lasterror()
	error(errxxx)
end

disp('<<< Timeout/Retry count test COMPLETED')
disp('')
disp('')
disp('Testing lcaGet/lcaPut/lcaPutNoWait')

end
// Test Put and Get
// Fill a matrix with numbers 1..10000

nums = ones(10,1)*[0:99] + [1:100:1000]'*ones(1,100);
wavs = {...
	'lca:wav0'; 'lca:wav1'; 'lca:wav2'; 'lca:wav3'; 'lca:wav4';...
	'lca:wav5'; 'lca:wav6'; 'lca:wav7'; 'lca:wav8'; 'lca:wav9';...
};

try

disp('CHECKING TIMESTAMPS')
lcaPut( 'lca:scl0', 432 )
[got, ts]  = lcaGet( 'lca:scl0' );
sleep(1000*3)
lcaPut( 'lca:scl0', 234 )
[got, ts1] = lcaGet( 'lca:scl0' );
got = real(ts1-ts);
if got < 2.5 | got > 3.5
	error('TIMESTAMP TEST FAILED')
end
disp('<<<OK')

lcaPut( wavs, nums );

[got, ts] = lcaGet(wavs);
if ( norm(got - nums) ~= 0 )
	error('lcaGet(wavs) ~= nums')
end

disp('VERIFY THAT lcaPutNoWait FLUSHES IMMEDIATELY')
lcaPutNoWait(wavs, zeros(10,100))

disp('Sleeping for 3s to verify lcaPutNoWait() flushing queue')
// don't change format of sleep; sed script for
// conversion to matlab looks for string
// 's','l','e','e','p','(','1','0','0','0','*'
sleep(1000*3)

[got, ts1] = lcaGet(wavs(1),1);

if ( prod(size(got)) ~= 1 )
	error('Asking for 1 waveform element failed')
end
if ( real(ts1-ts(1)) > 1 ) 
	error('lcaPutNoWait() flush check FAILED')
end
disp('<<<OK')

disp('CHECKING TYPE CONVERSIONS FOR lcaPut')

// restore values
lcaPutNoWait(wavs,nums)

// Verify that we can read a subarray
got = lcaGet(wavs,4);

if ( norm(got - nums(:,1:4)) ~= 0 )
	error('Reading subarray failed')
end

// Verify that we can write typed values
lcaPut('lca:scl0',2^32+1234,'d')
if ( lcaGet('lca:scl0') ~= 2^32+1234 )
	error('type DOUBLE readback check failed')
end

lcaPut('lca:scl0',2^32+1234,'l')
longscl = lcaGet('lca:scl0');
// FIXME: unclear how double is converted -> long
// some compilers produce 2^31-1 others -2^31...
if ( longscl ~= 2^31-1 & longscl ~= -2^31 )
	error('type LONG readback overflow check failed')
end
lcaPut('lca:scl0',2^16+1234,'l')
if ( lcaGet('lca:scl0') ~= 2^16+1234 )
	error('type LONG readback check failed')
end
lcaPut('lca:scl0',2^16+1234,'s')
if ( lcaGet('lca:scl0') ~= 1234 )
	error('type SHORT readback overflow check failed')
end
lcaPut('lca:scl0',1234,'s')
if ( lcaGet('lca:scl0') ~= 1234 )
	error('type SHORT readback check failed')
end
lcaPut('lca:scl0',1234,'b')
// IOC puts CHAR as UCHAR !!!
// (see) dbPutNotifyMapType() hence 
// well get 210 back instead of -46
if ( lcaGet('lca:scl0') ~= 1234-1024 )
	error('type BYTE readback overflow check failed')
end
lcaPut('lca:scl0',123,'b')
if ( lcaGet('lca:scl0') ~= 123 )
	error('type BYTE readback check failed')
end

disp('<<<OK')

disp('CHECKING TYPE CONVERSIONS FOR lcaGet')

// Verify that we can read typed values
lcaPut('lca:scl0', 2^32+1234)
// ??? conversion happens on IOC; seems we get -2^31
if ( lcaGet('lca:scl0',0,'l') ~= -2^31 )
	error('type LONG read overflow check failed')
end

lcaPut('lca:scl0',2^16+1234)
if ( lcaGet('lca:scl0',0,'l') ~= 2^16+1234 )
	error('type LONG read check failed')
end
// ??? conversion happens on IOC; seems we get -2^15
if ( lcaGet('lca:scl0',0,'s') ~= -2^15 )
	error('type SHORT read overflow check failed')
end
lcaPut('lca:scl0',1234)
if ( lcaGet('lca:scl0',0,'s') ~= 1234 )
	error('type SHORT read check failed')
end
// ??? conversion happens on IOC; bytes don't seem to
// saturate ???
if ( lcaGet('lca:scl0',0,'b') ~= -46 )
	error('type BYTE read overflow check failed')
end
lcaPut('lca:scl0',123)
if ( lcaGet('lca:scl0',0,'b') ~= 123 )
	error('type BYTE read check failed')
end

catch
	disp('Testing lcaGet/lcaPut/lcaPutNoWait FAILED')
	errxxx=lasterror()
	error(errxxx)
end

disp('<<<OK')

disp('CHECKING lcaGetNelem')
if ( prod( lcaGetNelem(wavs) == 100*ones(10,1) ) ~= 1 )
	error('lcaGetNelem FAILED')
end
disp('<<<OK')


disp('<< ALL DONE')
