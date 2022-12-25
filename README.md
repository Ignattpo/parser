# Описание    
Пример подключения:      
~$ telnet 127.0.0.1 3015      
Trying 127.0.0.1...      
Connected to 127.0.0.1.      
Escape character is '^]'.      
Hello      
Command line or File? (Enter C or F):       
Необходимо выбрать режим работы:        
**c,C**-команды     
**f,F**-файл со списками команд   

## Команды
*имя глобольной переменной/функции необходимо писать  в ""*   
*имя локальной переменной необходимо писать без ""*     

### ;
вывод результата
*Примеры использования:*      
```
a=malloc(4)
a;
0x7ff4b4001030 140688968650800
*a=52
*a;
0x34 52

```

### resolv  
резолвим имя переменной/функции     
*Примеры использования:*      
```
a = resolv "malloc"
Symbol 'malloc' at 0x7f42991660f0
resolv a
Address '7f42991660f0' located at /lib/x86_64-linux-gnu/libc.so.6 within the program __libc_malloc

```

### upload  
читаем из указателя данные по типам (u8 u16 u32)
*Примеры использования:*      
```
a=0xafbbccaa
upload u8 &a
Address '7ff4bc001038' = 0xaa
upload u16 &a
Address '7ff4bc001038' = 0xccaa
upload u32 &a
Address '7ff4bc001038' = 0xafbbccaa

b=upload u32 &a
Address '7ff4bc001038' = 0xafbbccaa
b;
0xafbbccaa 2948320426

```
### download  
записываем в указателя данные по типам (u8 u16 u32)
*Примеры использования:*      
```
a=malloc(4)

download u8 a 0xaabbccdd
Writed address '7ff4b4000e00' = 0xdd
download u16 a 0xaabbccdd
Writed address '7ff4b4000e00' = 0xccdd
download u32 a 0xaabbccdd
Writed address '7ff4b4000e00' = 0xaabbccdd

```
### mem_dump 
читаем из указателя определенное количество данных
*Примеры использования:*      
```
a=malloc(16)
mem_dump a 16
0x02 0x03 0x04 0x06 0x01 0x03 0x1d 0x05 
0x06 0x07 0x08 0x0c 0x22 0x06 0x07 0x08
```

### mem_write 
записывает в указатель строку с данными
*Примеры использования:*      
```
a=malloc(16)
mem_write a "0xfa 3 4 6 1 3 29 5 6 7 8 12 34 6 7 8 "
Writed 16 bytes to address 7ff4b4000e20

```

### mem_dump_f  
читаем из указателя определенное количество данных и записывает их в файл
*Примеры использования:* 
```
mem_dump_f "dump" a 16
memory dump written to file dump
```

### assert  
остановливает выполнения скрипта если переменная/символ не удовлетворяют условиям
*Примеры использования:* 
```
a=2
assert(a==2)
assert(a==3)
ERROR: Assert occurred at line 0

```

### exit, ^]
отключает клиента и освобождает используемые им данные и поток
### reset 
сбрасывает режим работы интерпретатора после этого происходит повторный запрос на выбор режима
### log
выведет все команды которые были введены с начала подключения
### !! 
выведет предыдущую введенную команду
### !# 
выведет следующую введенную команду

### вызов встроенных функций       
*Примеры использования:* 
```
a=malloc(32)
strlen("asdf");
0x4 4

```

