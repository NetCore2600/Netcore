.PHONY: all clean netshark libnetsocket libnetpcap

# Default target
all: netshark

# Build Netshark
netshark:
	@echo "Building Netshark..."
	$(MAKE) -C Netshark
	@mv ./Netshark/netshark .

# Clean all
clean:
	@echo "Cleaning Netshark..."
	$(MAKE) -C Netshark clean

# Optional future hooks:
libnetsocket:
	@echo "Building libnetsocket..."
	$(MAKE) -C libnetsocket

libnetpcap:
	@echo "Building libnetpcap..."
	$(MAKE) -C libnetpcap
