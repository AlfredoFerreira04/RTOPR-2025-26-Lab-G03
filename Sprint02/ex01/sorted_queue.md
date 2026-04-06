# FIFO Queue

## `pop()` function

```python
FUNCTION pop(buffer)
    elem = first

    IF elem IS NULL
        RETURN 0

    COPY elem.buffer TO buffer
    first = NULL
    node = NEXT_NODE(elem) # gets the next node in sorted order

    IF there is a next node # node EXISTS
        first = node

    REMOVE elem FROM TREE
    FREE elem memory
END FUNCTION
```

## `push()` function

```python
FUNCTION push(buffer)
    ALLOCATE memory for elem
    IF allocation failed
        RETURN 0
    COPY buffer TO elem.buffer

    node = ROOT
    parent = NULL
    is_first = 1

    WHILE node IS NOT NULL
        parent = node
        entry = node data

        IF elem.buffer[0] is less than entry.buffer[0]
            node = LEFT CHILD
        ELSE 
            node = RIGHT CHILD
            is_first = 0
    END WHILE

    IF is_first is equal to 1
        first = elem
    
    INSERT elem INTO TREE AT parent
    REBALANCE TREE
    RETURN 1
END FUNCTION
```

## `proc_open()` function

```python
FUNCTION proc_open(inode, filp)
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
    PRINT "release" with process ID
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
    
    RETURN 0
END FUNCTION
```

## `proc_exit()` function

```python
FUNCTION proc_exit()
    REMOVE proc_entry
    PRINT "proc entry removed"

    node = FIRST NODE IN TREE
    WHILE node EXISTS
        tmp = nodes data
        REMOVE node FROM TREE
        FREE tmp
        node = FIRST NODE IN TREE
    END WHILE
END FUNCTION
```
