## Group: ##
Brandon Chan, Jacob Simons, Tuan Lo, Samuel Klosi, Fiona O'Leary

## Instructions: ##
1. Type"make all" to compile and generate all of the files.

    ```bash
    make all
    ```
2. Start the airport server with
    ```bash
    ./airport
    ```
3. Start the places server with 
    ```bash
    ./place
    ```
4. Start the client with the arguments [city] [state], as such:
    ``` bash
    ./client [city] [state]
    ```
    Example:
    ``` bash
    ./client Seattle Washington
    ```
***Note:**  The client will connect to automatically "localhost". There's no argument to specify the remote machine*

## Remove rpc files: ##
    make clean 
