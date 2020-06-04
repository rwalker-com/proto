__updirs=$(if $1,$(if $2,$(if $(subst /,,$(abspath $2)),$(or $(wildcard $2$1),$(call __updirs,$1,$2../))),$(call __updirs,$1,./)))

# tests
$(info "$(call __updirs)")
$(info "$(call __updirs,$(lastword $(MAKEFILE_LIST)))") # expect ./updirs.mak

all:;
