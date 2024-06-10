
**Make Service**
  ``` sudo python3 make_service.py```
**Note**
-   Service path : ```/etc/systemd/system/```
-   Get log : ```sudo journalctl -u <service_name>```
-   Stop service : ```sudo systemctl stop <service_name>```
-   Disable sevice : ```sudo systemctl disable <service_name>```
-   Remove service : ```sudo rm /etc/systemd/system/<service_name>.service```
-   Get last log : ```sudo journalctl --no-pager -n 10 -u <service_name>.service``
-   


**Example**
- ```sudo journalctl --no-pager -n 10 -u app.service```
- ```sudo journalctl --no-pager -n 10 -u main.service```
- ```sudo journalctl --no-pager -n 10 -u feature_manager.service```
- ```sudo journalctl --no-pager -n 10 -u mongo_docker.service```