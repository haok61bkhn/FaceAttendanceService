import os
import sys


def create_systemd_service(script_path, restart=True):
    service_name = os.path.basename(script_path).replace(".sh", "")
    current_dir = os.path.dirname(script_path)
    if restart:
        service_content = f"""
    [Unit]
    Description=Service for {service_name}
    After=network.target

    [Service]
    ExecStart=bash {script_path}
    WorkingDirectory={current_dir}
    StandardOutput=journal
    StandardError=journal
    Restart=always 
    User={os.getlogin()}
    Group={os.getlogin()}

[Install]
WantedBy=multi-user.target
"""
    else:
        service_content = f"""
    [Unit]
    Description=Service for {service_name}
    After=network.target
    
    [Service]
    ExecStart=bash {script_path}
    WorkingDirectory={current_dir}
    StandardOutput=journal
    StandardError=journal
    User={os.getlogin()}
    Group={os.getlogin()}
    
[Install]
WantedBy=multi-user.target
"""

    service_file_path = f"/etc/systemd/system/{service_name}.service"
    print(f"Creating service file at {service_file_path}")
    try:
        with open(service_file_path, "w") as service_file:
            service_file.write(service_content)

        # Reload systemd daemon
        os.system("systemctl daemon-reload")

        # Enable the service
        os.system(f"systemctl enable {service_name}")

        # Start the service
        os.system(f"systemctl start {service_name}")

        print(f"Service {service_name} created and started successfully.")
    except PermissionError:
        print("Permission denied. Please run the script with sudo.")
    except Exception as e:
        print(f"An error occurred: {e}")


if __name__ == "__main__":
    current_dir = os.path.dirname(os.path.realpath(__file__))

    # make mongo_docker service
    f = open("mongo_docker.sh", "w")
    f.write(f"cd {current_dir}/install && ")
    f.write("docker-compose up -d --remove-orphans")
    f.close()
    script_path = f"{current_dir}/mongo_docker.sh"
    os.system(f"chmod +x {script_path}")
    create_systemd_service(script_path, restart=False)

    # make feature_manager service
    f = open("feature_manager.sh", "w")
    f.write(f"cd {current_dir}\n")
    f.write(f"sleep 1 \n")
    f.write("./feature_manager")
    f.close()
    script_path = f"{current_dir}/feature_manager.sh"
    os.system(f"chmod +x {script_path}")
    create_systemd_service(script_path)

    # make main service
    f = open("main.sh", "w")
    f.write(f"cd {current_dir}\n")
    f.write(f"sleep 5 \n")
    f.write("./main")
    f.close()
    script_path = f"{current_dir}/main.sh"
    os.system(f"chmod +x {script_path}")
    create_systemd_service(script_path)

    # make app service
    f = open("app.sh", "w")
    f.write(f"cd {current_dir}\n")
    f.write(f"sleep 10 \n")
    f.write("./app")
    f.close()
    script_path = f"{current_dir}/app.sh"
    os.system(f"chmod +x {script_path}")
    create_systemd_service(script_path)
