set makensis=c:\progra~1\nsis\makensis /DUSE_MUI /Dlzma

%makensis% /Dfull /DFILES_PATH="c:\progra~1\winlamp" main
%makensis% /Dlite /DFILES_PATH="c:\progra~1\winlamp" main
%makensis% /Dfull /Dpro /DFILES_PATH="c:\progra~1\winlamp" main
%makensis% /Dfull /Dalkbundle /DeMusic-7plus /DFILES_PATH="c:\progra~1\winlamp" main
%makensis% /Dfull /DeMusic-7plus /DFILES_PATH="c:\progra~1\winlamp" main