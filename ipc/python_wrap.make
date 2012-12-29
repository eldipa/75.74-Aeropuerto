SELF = python_wrap.make
TARGET = ipc

all: $(TARGET).i ../os/libos.a ../log/liblog.a ../common/libcommon.a libipc.a
	@swig -c++ -Wall -python $(TARGET).i
	@g++ -c -fpic $(TARGET)_wrap.cxx -I/usr/include/python2.7 -I../os -I../log
	@g++ -shared $(TARGET)_wrap.o libipc.a ../os/libos.a ../log/liblog.a ../common/libcommon.a -o _$(TARGET).so

test:
	@python test.py

clean:
	@rm -f _$(TARGET).so
	@rm -f $(TARGET).pyc
	@rm -f $(TARGET)_wrap.cxx
	@rm -f $(TARGET).py
