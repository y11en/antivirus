call klsumfix.exe "%1" zero
call signer sign_reg -filter compid=AH,type=BASE -signset developer -kl ./kl.pbv -oper ./oper.pbv -output ./sign.reg -new -inputmask "%1"
call sigatt kl.pbv oper.pbv sign.reg "%1"
del kl.pbv
del oper.pbv
del sign.reg
call tsigner %1
call klsumfix.exe "%1" fix
