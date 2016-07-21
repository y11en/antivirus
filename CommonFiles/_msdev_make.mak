
make_target:
	@echo Building $(TARGET)
	@msdev "$(DSW).dsw" /MAKE "$(TARGET)" $(REBUILD)

