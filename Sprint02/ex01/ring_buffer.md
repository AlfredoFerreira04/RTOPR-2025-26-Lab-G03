# Ring Buffer

## `increment()` function

```python
FUNCTION increment(item_pointer)
    ret = item_pointer
    item_pointer = (item_pointer + 1) MODULO QUEUE_LEN
    RETURN item
END FUNCTION
```

## `is_empty()` function

```python
FUNCTION is_empty(read, write)
    IF read is equal to write
        RETURN TRUE
    Else
        RETURN FALSE
END FUNCTION
```

## `is_full()` function

```python
FUNCTION is_full(read, write)
    next_write = (write + 1) MODULO QUEUE_LEN

    IF next_write is equal to read
        RETURN TRUE
    ELSE 
        RETURN FALSE
END FUNCTION
```

## `dequeue()` function

```python
FUNCTION dequeue(buffer)
    IF NOT is_empty(ring.read_item, ring.write_item)
        COPY ring.queue[ring.read_item].buffer TO buffer
        increment(ring.read_item)
        RETURN 1
    RETURN 0
END FUNCTION
```

## `enqueue()` function

```python
FUNCTION enqueue(buffer)
    IF is_full(ring.read_item, ring.write_item)
        increment(ring.read_item)
    COPY buffer TO ring.queue[ring.write_item].buffer
    increment(ring.write_item)
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
FUNCTION proc_read(file, user_buffer, count, position)
    DECLARE buffer[BUFFER_LEN]
    PRINT "read" with process ID

    IF position is greater than len
        position = 0 
        RETURN 0

    ret = dequeue(buffer)

    IF ret is less than or equal to 0
        RETURN ret

    len = LENGTH of buffer

    IF len is less than or equal to 0
        RETURN ERROR

    IF count is less than len
        RETURN ERROR

    COPY buffer TO user_buffer

    IF copy failed 
        RETURN ERROR
    
    position = position + (count -len)

    RETURN len
END FUNCTION
```

## `proc_write()` function

```python
FUNCTION proc_write(file, user_buffer, count, position)

    DECLARE buffer[BUFFER_LEN]

    PRINT "write" with process ID

    IF count is greater than BUFFER_LEN
        RETURN INVALID

    COPY user_buffer TO buffer

    IF copy failed
        RETURN ERROR

    buffer[count] = END_OF_STRING

    ret = enqueue(buffer)

    IF ret is less than or equal to 0
        RETURN ret

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
    ring.write_item = 0
    ring.read_item = 0
    RETURN 0
END FUNCTION
```

## `proc_exit()` function

```python
FUNCTION proc_exit()
    REMOVE proc_entry
    PRINT "proc entry removed"
END FUNCTION
```
