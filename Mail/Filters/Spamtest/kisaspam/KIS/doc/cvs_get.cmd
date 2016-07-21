@echo off
cls

echo. > cvs_get.log

cvs get -r as-20-for-kis-branch-1 gnu            >> cvs_get.log
cvs get -r as-20-for-kis-branch-1 smtp-filter    >> cvs_get.log
cvs get -r as-20-for-kis-branch-1 lib            >> cvs_get.log
cvs get -r as-20-for-kis-branch-1 MIMEParser     >> cvs_get.log
cvs get -r as-20-for-kis-branch-1 kis            >> cvs_get.log
cvs get -r as-20-for-kis-branch-1 client-filter  >> cvs_get.log
cvs get -r as-20-for-kis-branch-1 _include       >> cvs_get.log
cvs get -r as-20-for-kis-branch-1 Filtration     >> cvs_get.log
cvs get -r as-20-for-kis-branch-1 libmorph       >> cvs_get.log
cvs get -r as-20-for-kis-branch-1 ZTools         >> cvs_get.log

REM TODO: change -A to as-31-for-kis-branch-2

cvs get -A                        kis                       >> cvs_get.log
cvs get -A                        smtp-filter/libgsg2       >> cvs_get.log
cvs get -A                        smtp-filter/libsigs       >> cvs_get.log
cvs get -A                        smtp-filter/libosr        >> cvs_get.log
cvs get -A                        gnu/libpng                >> cvs_get.log
cvs get -A                        gnu/zlib                  >> cvs_get.log
cvs get -A                        gnu/jpeg                  >> cvs_get.log
cvs get -A                        smtp-filter/ungif         >> cvs_get.log
