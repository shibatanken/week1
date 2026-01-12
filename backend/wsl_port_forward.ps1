# WSL Port Forwarding Script for LAN Access
# This script forwards WSL port 8081 to Windows host port 8081 on all interfaces

Write-Host "Configuring WSL port forwarding for LAN access..." -ForegroundColor Cyan

# Get WSL IP address
$wslIP = (wsl hostname -I).Trim()
Write-Host "WSL IP: $wslIP" -ForegroundColor Yellow

# Remove existing rule if exists
netsh interface portproxy delete v4tov4 listenport=8081 listenaddress=0.0.0.0 | Out-Null

# Add port forwarding rule: Forward Windows port 8081 to WSL port 8081
netsh interface portproxy add v4tov4 listenport=8081 listenaddress=0.0.0.0 connectport=8081 connectaddress=$wslIP

# Add Windows Firewall rule to allow port 8081
Write-Host "Configuring Windows Firewall..." -ForegroundColor Cyan
netsh advfirewall firewall delete rule name="WSL Port 8081" | Out-Null
netsh advfirewall firewall add rule name="WSL Port 8081" dir=in action=allow protocol=TCP localport=8081

Write-Host ""
Write-Host "Port forwarding configured successfully!" -ForegroundColor Green
Write-Host "Server is now accessible from LAN at: $((ipconfig | Select-String -Pattern 'IPv4.*192\.168' | Select-Object -First 1) -replace '.*:\s*', ''):8081" -ForegroundColor Yellow
Write-Host ""
Write-Host "To view port forwarding rules: netsh interface portproxy show all" -ForegroundColor Gray
Write-Host "To remove forwarding: netsh interface portproxy delete v4tov4 listenport=8081 listenaddress=0.0.0.0" -ForegroundColor Gray

