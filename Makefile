ifeq (,$(filter _%,$(notdir $(CURDIR))))
  include target.mk
else
  include $(SRCDIR)/rules.mk
endif
