#!/bin/csh
set user = $1
set Expected = HelloFrom.pborkar6.yZuVQHNHAPGYtauwqbAwDcWsyxEPiWqtfEyzOXyRl
set Actual = `./BreakRSA  4700959521550369877 4403072379102577927  3195611530678673470 3709353493891259314 3690858851357108368 2157808533890365006 2178848837328126647 2336912049589753401 3603895693404238954 3354149763370723130 4539751668021164126 3322759248172421130`
echo "expected is $Expected" 
echo "actal    is $Actual"
if ( "$Expected" == "$Actual" ) then
echo "Grade for user $user is 100"
else
echo "Grade for user $user is 50"
endif
