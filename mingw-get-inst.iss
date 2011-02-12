; This InnoSetup installer script is
; Copyright (c) 2010 Charles S. Wilson
;
; Permission is hereby granted, free of charge, to any person obtaining a
; copy of this software and associated documentation files (the "Software"),
; to deal in the Software without restriction, including without limitation
; the rights to use, copy, modify, merge, publish, distribute, sublicense,
; and/or sell copies of the Software, and to permit persons to whom the
; Software is furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included
; in all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
; OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
; THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
; OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
; ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
; OTHER DEALINGS IN THE SOFTWARE.
;
; Note that ONLY this InnoSetup installer is licensed as above. The
; applications that it installs carry their own licensing terms (mostly
; GPL).

; To build this installer, first download mingw-get itself, and unpack into
; the _inst subdirectory of the directory in which this file is located.
;
;   $ V=0.1-mingw32-alpha-3
;   $ rm -rf _inst && mkdir _inst
;   $ for f in mingw-get-$V-bin.tar.gz mingw-get-$V-lic.tar.gz pkginfo-$V-bin.tar.gz; do
;       wget --no-check-certificate $f
;       tar -C _inst -xf $f
;     done
;
; Then, we need to move some files around.  First, there is a bug in the -lic
; package:
;
;   $ if [ -d _inst/shared ]; then
;       if [ ! -d _inst/share ]; then
;         mv _inst/shared _inst/share
;       else
;         mv _inst/shared/* _inst/share/
;         rmdir _inst/shared
;       fi
;     fi
;
; Copy defaults.xml to profile.xml
;
;   $ pushd _inst/var/lib/mingw-get/data/
;   $ cp defaults.xml profile.xml
;   $ popd
;
; Now, you should be able to do the following
;
;   $ cd _inst/bin
;   $ ./mingw-get update
;
; to download the latest copy of all of the published xml manifests.
;
; NOW, you're ready to compile this installer!
; Don't forget to rename it with the release date.

#define MyAppName "MinGW-Get"
#define MyAppVersion "0.1-alpha-3"
#define MyAppPublisher "MinGW"
#define MyAppURL "http://www.mingw.org/"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{AC2C1BDB-1E91-4F94-B99C-E716FE2E9C75}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName=C:\MinGW
DefaultGroupName=MinGW
AllowNoIcons=yes
LicenseFile=_inst/share/doc/mingw-get/COPYING
OutputDir=output
OutputBaseFilename=mingw-get-inst
SetupIconFile=msys.ico
Compression=lzma
SolidCompression=yes
PrivilegesRequired=admin

[Languages]
Name: english; MessagesFile: compiler:Default.isl

[Files]
Source: _inst\*; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
; NOTE: only install shortcut to MSYS shell if msys was installed
Name: {group}\MinGW Shell; Filename: {app}\msys\1.0\msys.bat; WorkingDir: {app}\msys\1.0\bin; IconFilename: {app}\msys\1.0\msys.ico; Check: CheckMSYSSelected
Name: {group}\Uninstall mingw-get; Filename: {uninstallexe}; Comment: "Only removes the mingw-get tool; not MinGW"

[Run]
Filename: {app}\bin\mingw-get.exe; Parameters: {code:GetMinGWGetArgs}

[Code]
var
  Page: TWizardPage;
  CheckListBox: TNewCheckListBox;

  C_Index: Integer;
  CXX_Index: Integer;
  Java_Index: Integer;
  F_index: Integer;
  ObjC_Index: Integer;
  Ada_Index: Integer;
  Msys_Index: Integer;

procedure CreateTheWizardPages;
begin
  Page := CreateCustomPage(wpSelectTasks, 'Select Components', 'Choose which optional components of MinGW to install (the C compiler is always installed)');

  CheckListBox := TNewCheckListBox.Create(Page);
  CheckListBox.Width := Page.SurfaceWidth;
  CheckListBox.Height := ScaleY(97);
  CheckListBox.Flat := True;
  CheckListBox.Parent := Page.Surface;
  CheckListBox.AddCheckBox('MinGW Compiler Suite', '', 0, True, True, True, False, nil);
  C_Index :=    CheckListBox.AddCheckBox('C Compiler',       '', 1, True,  False, False, False, nil);
  CXX_Index :=  CheckListBox.AddCheckBox('C++ Compiler',     '', 1, False, True,  False, True, nil);

  { FIXME: no java package at present }
  { Java_Index := CheckListBox.AddCheckBox('Java Compiler',    '', 1, False, True,  False, True, nil); }

  F_index :=    CheckListBox.AddCheckBox('Fortran Compiler', '', 1, False, True,  False, True, nil);
  ObjC_Index := CheckListBox.AddCheckBox('ObjC Compiler',    '', 1, False, True,  False, True, nil);
  Ada_Index :=  CheckListBox.AddCheckBox('Ada Compiler',     '', 1, False, True,  False, True, nil);
  Msys_Index := CheckListBox.AddCheckBox('MSYS Basic System','', 0, False, True,  False, False, nil);
end;

procedure InitializeWizard;
begin
  CreateTheWizardPages
end;

function GetMinGWGetArgs(Param: String): String;
var
  args: String;
begin
  { Return the selected DataDir }
  args := 'install base ';

  { Don't need to do this, because 'base' takes care of it }
  { if CheckListBox.Checked[C_Index] then begin }
  {   args := args +  'gcc-core '; }
  { end }

  if CheckListBox.Checked[CXX_Index] then begin
    args := args +  'gcc-g++ ';
  end

  { FIXME: no java package at present }
  { if CheckListBox.Checked[Java_Index] then begin }
  {   args := args +  'gcc-java '; }
  { end }

  if CheckListBox.Checked[F_Index] then begin
    args := args +  'gcc-fortran ';
  end
  if CheckListBox.Checked[ObjC_Index] then begin
    args := args +  'gcc-objc ';
  end
  if CheckListBox.Checked[Ada_Index] then begin
    args := args +  'gcc-ada ';
  end
  if CheckListBox.Checked[Msys_Index] then begin
    args := args +  'msys-base ';
  end
  Result := args;
end;

function CheckMSYSSelected: Boolean;
begin
  Result := CheckListBox.Checked[Msys_Index];
end;

function UpdateReadyMemo(Space, NewLine, MemoUserInfoInfo, MemoDirInfo, MemoTypeInfo,
  MemoComponentsInfo, MemoGroupInfo, MemoTasksInfo: String): String;
var
  S: String;
begin
  { Fill the 'Ready Memo' with the normal settings and the custom settings }
  S := '';
  S := S + 'Installing:' + NewLine;
  if CheckListBox.Checked[C_Index] then begin
    S := S + Space + 'C Compiler' + NewLine;
  end
  if CheckListBox.Checked[CXX_Index] then begin
    S := S + Space + 'C++ Compiler' + NewLine;
  end

  { FIXME: no java package at present }
  { if CheckListBox.Checked[Java_Index] then begin }
  {   S := S + Space + 'Java Compiler' + NewLine; }
  { end }

  if CheckListBox.Checked[F_Index] then begin
    S := S + Space + 'Fortran Compiler' + NewLine;
  end
  if CheckListBox.Checked[ObjC_Index] then begin
    S := S + Space + 'ObjC Compiler' + NewLine;
  end
  if CheckListBox.Checked[Ada_Index] then begin
    S := S + Space + 'Ada Compiler' + NewLine;
  end
  if CheckListBox.Checked[Msys_Index] then begin
    S := S + Space + 'MSYS Basic System' + NewLine;
  end
  S := S + NewLine;

  S := S + MemoDirInfo + NewLine;
  Result := S;
end;

procedure DoPostInstall();
var
  fstabFileName: String;
  fstabEntry: String;
begin
  if CheckMSYSSelected() then
  begin
    fstabFileName := ExpandConstant('{app}\msys\1.0\etc\fstab');
    fstabEntry := ExpandConstant('{app}\') + '   /mingw' + #13#10;
    SaveStringToFile(fstabFileName, fstabEntry, False);
  end
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    DoPostInstall();
  end;
end;