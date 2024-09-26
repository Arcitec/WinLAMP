cd /D %CURSANDBOX%\output\WinLAMP
dir /b /s >files.txt
rebase -b 7000000 @files.txt -l rebase.log