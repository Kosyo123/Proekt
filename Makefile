CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
SRCDIR   = src
SOURCES  = $(SRCDIR)/Utils.cpp \
           $(SRCDIR)/Enums.cpp \
           $(SRCDIR)/BaseEntity.cpp \
           $(SRCDIR)/User.cpp \
           $(SRCDIR)/ActivityLog.cpp \
           $(SRCDIR)/Task.cpp \
           $(SRCDIR)/Sorter.cpp \
           $(SRCDIR)/Project.cpp \
           $(SRCDIR)/TaskManager.cpp \
           main.cpp
TARGET   = task_manager

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

.PHONY: all clean
