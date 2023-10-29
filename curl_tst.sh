#!/bin/bash

# Web server URL and port to test
SERVER_URL="http://127.0.0.1"
PORT=8080  # Change this to your desired port number

# Number of clients (n) and number of requests per client (m)
NUM_CLIENTS=100
REQUESTS_PER_CLIENT=50

# Counter for successful responses
SUCCESSFUL_RESPONSES=0

# Loop to run curl requests from multiple clients
for ((i=1; i<=$NUM_CLIENTS; i++)); do
    for ((j=1; j<=$REQUESTS_PER_CLIENT; j++)); do
        # Send a GET request using curl with the custom port
        HTTP_RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" "$SERVER_URL:$PORT")

        # Check if the response code is 200 (successful)
        if [ "$HTTP_RESPONSE" -eq 200 ]; then
            SUCCESSFUL_RESPONSES=$((SUCCESSFUL_RESPONSES + 1))
        fi

        # Echo the iteration with client number and request number
        echo "Client $i, Request $j: Response Code $HTTP_RESPONSE"
    done
done

# Calculate success rate
SUCCESS_RATE=$(echo "scale=2; $SUCCESSFUL_RESPONSES / ($NUM_CLIENTS * $REQUESTS_PER_CLIENT) * 100" | bc)

# Print results
echo "Total requests sent: $((NUM_CLIENTS * REQUESTS_PER_CLIENT))"
echo "Successful responses: $SUCCESSFUL_RESPONSES"
echo "Success rate: $SUCCESS_RATE%"
