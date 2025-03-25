# short-URL
Create a short URL service in C++.

## Steps to run the shortenUrl service:
1. Clone the repository.
2. Run `git submodule update --init --recursive`.
3. From the build folder, run `cmake ..`.
4. Run `make`.
5. Run `./shortenUrl`.

## Testing
I used `wrk` to test parallel requests for reads, as reads are more frequent than writes. Reads redirect to the original URL. Below is the output:

```sh
wrk -t4 -c100 -d30s http://localhost:8080/omdJuw
Running 30s test @ http://localhost:8080/omdJuw
    4 threads and 100 connections
    Thread Stats   Avg      Stdev     Max   +/- Stdev
        Latency    49.45ms    8.52ms 107.00ms   88.93%
        Req/Sec   507.21     67.87   656.00     76.92%
    60727 requests in 30.07s, 17.10MB read
Requests/sec:   2019.19
Transfer/sec:    582.31KB
```

- **Latency**: The average time taken for a request to be processed, measured in milliseconds (ms). Lower latency indicates faster response times.
  - **49.45ms**: The average latency for the requests.
  - **8.52ms**: The standard deviation of the latency, indicating variability.

- **Requests/sec**: The total number of requests processed per second during the test.
  - **2019.19**: The total number of requests processed per second by all threads combined.

- **Transfer/sec**: The amount of data transferred per second, measured in kilobytes (KB).
  - **582.31KB**: The average amount of data transferred per second.

This output shows that the service can handle a high number of requests with relatively low latency.
