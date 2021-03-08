# ============================================================================
#  Name	 : build_help.mk
#  Part of  : AquariumControl
# ============================================================================
#  Name	 : build_help.mk
#  Part of  : AquariumControl
#
#  Description: This make file will build the application help file (.hlp)
# 
# ============================================================================

do_nothing :
	@rem do_nothing

# build the help from the MAKMAKE step so the header file generated
# will be found by cpp.exe when calculating the dependency information
# in the mmp makefiles.

MAKMAKE : AquariumControl_0xa291b5a5.hlp
AquariumControl_0xa291b5a5.hlp : AquariumControl.xml AquariumControl.cshlp Custom.xml
	cshlpcmp.bat AquariumControl.cshlp
ifeq (WINSCW,$(findstring WINSCW, $(PLATFORM)))
	md $(EPOCROOT)epoc32\$(PLATFORM)\c\resource\help
	copy AquariumControl_0xa291b5a5.hlp $(EPOCROOT)epoc32\$(PLATFORM)\c\resource\help
endif

BLD : do_nothing

CLEAN :
	del AquariumControl_0xa291b5a5.hlp
	del AquariumControl_0xa291b5a5.hlp.hrh

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo AquariumControl_0xa291b5a5.hlp

FINAL : do_nothing
