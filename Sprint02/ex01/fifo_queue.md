# FIFO Queue

## `pop()` function

```python
FUNCTION pop(buffer)
    IF queue is empty
        RETURN 0
    node_queue = first element of list
    COPY node_queue.buffer TO buffer
    REMOVE node_queue from list
    FREE node_queue memory
    RETURN 1
END FUNCTION
```

## `push()` function

```python
FUNCTION push(buffer)
    ALLOCATE memory for node_queue
    IF allocation failed
        RETURN 0
    COPY buffer TO node_queue.buffer
    ADD node_queue to end of list
    RETURN 1
END FUNCTION
```

## `proc_open()` function

```python
FUNCTION proc_open(inode, file)
    PRINT "open" with process ID
    RETURN 0
END FUNCTION
```

## `proc_read()` function

```python
FUNCTION proc_read(filp, buf, count, f_pos)
    DECLARE buffer[BUFFER_LEN]
    PRINT "read" with process ID

    IF f_pos is greater than 0
        RETURN 0
    CALL pop(buffer)
    IF pop failed
        RETURN 0
    
    len = LENGTH of buffer
    
    IF len is less than or equal to 0
        RETURN ERROR
    IF count is less than len
        RETURN ERROR

    COPY buffer to buf
    IF copy failed
        RETURN ERROR
    
    f_pos = f_pos + (count - len)

    RETURN len
END FUNCTION
```

## `proc_write()` function

```python
FUNCTION proc_write(filp, buf, count, f_pos)
    DECLARE buffer[BUFFER_LEN]
    PRINT "write" with process ID

    IF count is greater than BUFFER_LEN
        RETURN INVALID

    COPY buf TO buffer

    IF copy failed
        RETURN ERROR

    ADD null terminator to buffer
    CALL push(buffer)
    IF push failed
        RETURN ERROR

    RETURN count
END FUNCTION
```

## `proc_close()` function

```python
FUNCTION proc_close(inode, file)
    PRINT "close" with process ID
    RETURN 0
END FUNCTION
```

## `proc_init()` function

```python
FUNCTION proc_init()
    CREATE proc_entry with name ENTRY_NAME
    IF creation failed
        RETURN NO MEMORY ERROR
    PRINT "proc entry created"
    INITIALIZE empty list head
    RETURN 0
END FUNCTION
```

## `proc_exit()` function

```python
FUNCTION proc_exit()
    REMOVE proc_entry
    PRINT "proc entry removed"

    FOR EACH node in list
        REMOVE node from list
        FREE node memory
    END FOR
END FUNCTION
```
