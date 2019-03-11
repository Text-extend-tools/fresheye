# Microsoft Developer Studio Project File - Name="recode" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Generic Project" 0x010a

CFG=recode - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "recode.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "recode.mak" CFG="recode - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "recode - Win32 Release" (based on "Win32 (x86) Generic Project")
!MESSAGE "recode - Win32 Debug" (based on "Win32 (x86) Generic Project")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
MTL=midl.exe

!IF  "$(CFG)" == "recode - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "recode - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "recode___Win32_Debug"
# PROP BASE Intermediate_Dir "recode___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "recode - Win32 Release"
# Name "recode - Win32 Debug"
# Begin Group "Source files"

# PROP Default_Filter ".koi8-r"
# Begin Source File

SOURCE="..\src\fe.koi8-r"

!IF  "$(CFG)" == "recode - Win32 Release"

# Begin Custom Build
InputDir=\cygwin\home\vadimp\src\fe\src
OutDir=.\Release
InputPath=..\src\fe.koi8-r
InputName=fe

"$(InputDir)\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(OutDir)\ce" -i koi8-r < "$(InputDir)\$(InputName).koi8-r" > "$(InputDir)\$(InputName)".c

# End Custom Build

!ELSEIF  "$(CFG)" == "recode - Win32 Debug"

# Begin Custom Build
InputDir=\cygwin\home\vadimp\src\fe\src
OutDir=.\Debug
InputPath=..\src\fe.koi8-r
InputName=fe

"$(InputDir)\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(OutDir)\ce" -i koi8-r < "$(InputDir)\$(InputName).koi8-r" > "$(InputDir)\$(InputName)".c

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\src\lingtbl.koi8-r"

!IF  "$(CFG)" == "recode - Win32 Release"

# Begin Custom Build
InputDir=\cygwin\home\vadimp\src\fe\src
OutDir=.\Release
InputPath=..\src\lingtbl.koi8-r
InputName=lingtbl

"$(InputDir)\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(OutDir)\ce" -i koi8-r < "$(InputDir)\$(InputName).koi8-r" > "$(InputDir)\$(InputName)".c

# End Custom Build

!ELSEIF  "$(CFG)" == "recode - Win32 Debug"

# Begin Custom Build
InputDir=\cygwin\home\vadimp\src\fe\src
OutDir=.\Debug
InputPath=..\src\lingtbl.koi8-r
InputName=lingtbl

"$(InputDir)\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(OutDir)\ce" -i koi8-r < "$(InputDir)\$(InputName).koi8-r" > "$(InputDir)\$(InputName)".c

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\src\ui.koi8-r"

!IF  "$(CFG)" == "recode - Win32 Release"

# Begin Custom Build
InputDir=\cygwin\home\vadimp\src\fe\src
OutDir=.\Release
InputPath=..\src\ui.koi8-r
InputName=ui

"$(InputDir)\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(OutDir)\ce" -i koi8-r < "$(InputDir)\$(InputName).koi8-r" > "$(InputDir)\$(InputName)".c

# End Custom Build

!ELSEIF  "$(CFG)" == "recode - Win32 Debug"

# Begin Custom Build
InputDir=\cygwin\home\vadimp\src\fe\src
OutDir=.\Debug
InputPath=..\src\ui.koi8-r
InputName=ui

"$(InputDir)\$(InputName).c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(OutDir)\ce" -i koi8-r < "$(InputDir)\$(InputName).koi8-r" > "$(InputDir)\$(InputName)".c

# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
