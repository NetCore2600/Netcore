# NETCORE 2600

```bash
███╗   ██╗███████╗████████╗ ██████╗ ██████╗ ██████╗ ███████╗    ██████╗  ██████╗  ██████╗  ██████╗ 
████╗  ██║██╔════╝╚══██╔══╝██╔════╝██╔═══██╗██╔══██╗██╔════╝    ╚════██╗██╔════╝ ██╔═████╗██╔═████╗
██╔██╗ ██║█████╗     ██║   ██║     ██║   ██║██████╔╝█████╗       █████╔╝███████╗ ██║██╔██║██║██╔██║
██║╚██╗██║██╔══╝     ██║   ██║     ██║   ██║██╔══██╗██╔══╝      ██╔═══╝ ██╔═══██╗████╔╝██║████╔╝██║
██║ ╚████║███████╗   ██║   ╚██████╗╚██████╔╝██║  ██║███████╗    ███████╗╚██████╔╝╚██████╔╝╚██████╔╝
╚═╝  ╚═══╝╚══════╝   ╚═╝    ╚═════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝    ╚══════╝ ╚═════╝  ╚═════╝  ╚═════╝ 
```

## Installation

1. **Install Dependencies**

   - Install the PCAP development library:
     ```bash
     sudo apt-get install libpcap-dev  # For Debian/Ubuntu
     sudo yum install libpcap-devel    # For CentOS/RHEL
     ```

2. **Build the Project**

   ```bash
   make
   ```

3. **Run the Program**
   ```bash
   ./netshark -i <interface> -f <filter>
   ```

Note: The program requires root privileges to capture network packets. Run it with `sudo` if needed.
# Netcore
