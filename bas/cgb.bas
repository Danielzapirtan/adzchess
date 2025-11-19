10 rem Goldbach
20 let ki=1024
30 let mi=ki*ki
32 let n=64*mi
33 let nmax=128*mi
40 if n>nmax then 80
50 goto 1000
60 let s=mi
61 if n>n/s*s then 65
62 print "ok ";n/mi;"M"
65 let n=n+2
70 goto 40
80 print "ok ";n-2
90 end
900 print "Counterexample: ";n
910 end
1000 let p=3
1010 if p>n-p then 900
1020 let r=p
1030 let pr=#r
1035 if pr=0 then 1080
1040 let r=n-r
1050 if r=p then 60
1060 goto 1030
1070 goto 60
1080 let p=p+2
1090 goto 1010
