##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=Prj2
ConfigurationName      :=Debug
WorkspacePath          := "/home/debian/Desktop/CodeLite_Workspace"
ProjectPath            := "/home/debian/Desktop/CodeLite_Workspace/Prj2"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=K5M
Date                   :=04/22/19
CodeLitePath           :="/home/debian/.codelite"
LinkerName             :=/usr/bin/g++ 
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="Prj2.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  -lpthread  -lrt
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++ 
CC       := /usr/bin/gcc 
CXXFLAGS :=  -g -O0 -Wall $(Preprocessors)
CFLAGS   := -std=c11 -g -O0 -Wall -D_GNU_SOURCE $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as 


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/main.c$(ObjectSuffix) $(IntermediateDirectory)/Master_Functions.c$(ObjectSuffix) $(IntermediateDirectory)/TivaComm_Thread.c$(ObjectSuffix) $(IntermediateDirectory)/My_UART_BB.c$(ObjectSuffix) $(IntermediateDirectory)/Logger_Thread.c$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main.c$(ObjectSuffix): main.c $(IntermediateDirectory)/main.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/debian/Desktop/CodeLite_Workspace/Prj2/main.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.c$(DependSuffix): main.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.c$(ObjectSuffix) -MF$(IntermediateDirectory)/main.c$(DependSuffix) -MM "main.c"

$(IntermediateDirectory)/main.c$(PreprocessSuffix): main.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.c$(PreprocessSuffix) "main.c"

$(IntermediateDirectory)/Master_Functions.c$(ObjectSuffix): Master_Functions.c $(IntermediateDirectory)/Master_Functions.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/debian/Desktop/CodeLite_Workspace/Prj2/Master_Functions.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Master_Functions.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Master_Functions.c$(DependSuffix): Master_Functions.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Master_Functions.c$(ObjectSuffix) -MF$(IntermediateDirectory)/Master_Functions.c$(DependSuffix) -MM "Master_Functions.c"

$(IntermediateDirectory)/Master_Functions.c$(PreprocessSuffix): Master_Functions.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Master_Functions.c$(PreprocessSuffix) "Master_Functions.c"

$(IntermediateDirectory)/TivaComm_Thread.c$(ObjectSuffix): TivaComm_Thread.c $(IntermediateDirectory)/TivaComm_Thread.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/debian/Desktop/CodeLite_Workspace/Prj2/TivaComm_Thread.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/TivaComm_Thread.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/TivaComm_Thread.c$(DependSuffix): TivaComm_Thread.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/TivaComm_Thread.c$(ObjectSuffix) -MF$(IntermediateDirectory)/TivaComm_Thread.c$(DependSuffix) -MM "TivaComm_Thread.c"

$(IntermediateDirectory)/TivaComm_Thread.c$(PreprocessSuffix): TivaComm_Thread.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/TivaComm_Thread.c$(PreprocessSuffix) "TivaComm_Thread.c"

$(IntermediateDirectory)/My_UART_BB.c$(ObjectSuffix): My_UART_BB.c $(IntermediateDirectory)/My_UART_BB.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/debian/Desktop/CodeLite_Workspace/Prj2/My_UART_BB.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/My_UART_BB.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/My_UART_BB.c$(DependSuffix): My_UART_BB.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/My_UART_BB.c$(ObjectSuffix) -MF$(IntermediateDirectory)/My_UART_BB.c$(DependSuffix) -MM "My_UART_BB.c"

$(IntermediateDirectory)/My_UART_BB.c$(PreprocessSuffix): My_UART_BB.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/My_UART_BB.c$(PreprocessSuffix) "My_UART_BB.c"

$(IntermediateDirectory)/Logger_Thread.c$(ObjectSuffix): Logger_Thread.c $(IntermediateDirectory)/Logger_Thread.c$(DependSuffix)
	$(CC) $(SourceSwitch) "/home/debian/Desktop/CodeLite_Workspace/Prj2/Logger_Thread.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Logger_Thread.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Logger_Thread.c$(DependSuffix): Logger_Thread.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Logger_Thread.c$(ObjectSuffix) -MF$(IntermediateDirectory)/Logger_Thread.c$(DependSuffix) -MM "Logger_Thread.c"

$(IntermediateDirectory)/Logger_Thread.c$(PreprocessSuffix): Logger_Thread.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Logger_Thread.c$(PreprocessSuffix) "Logger_Thread.c"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) ./Debug/*$(ObjectSuffix)
	$(RM) ./Debug/*$(DependSuffix)
	$(RM) $(OutputFile)
	$(RM) "../.build-debug/Prj2"


