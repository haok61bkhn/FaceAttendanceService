#!/bin/bash

# Number of parallel instances
NUM_INSTANCES=10

# Function to run deepstream-app
run_deepstream_app() {
    deepstream-app -c test_config/yolov8.txt
}

# Run instances in parallel
for ((i=1; i<=$NUM_INSTANCES; i++)); do
    run_deepstream_app &
done

# Wait for all instances to finish
wait
