## Input Data
'-' can be every character except digits
```
-3-312---
----32-31
2---11-23
212------
123-1---3
1--1----1
--0-12--1
---1-----
121--2-00
```
## Output Data
```
.   .   .   .___.   .   .___.   .___.   .
            | 3 |       |   |   | 3 |    
.   .___.___!   !   .___!   !___!   !___.
    | 2   1     | 2 | 3   0   2         |
.___!   .   .   !   !___.   .___.___.___!
|         0     |       |   |         2  
!   .___.   .   !   .___!   !___.___.___.
|   |   | 1     | 3 |     0             |
!___!   !   .   !___!   .   .   .___.   !
  1     |                       | 3 | 3 |
.   .   !   .   .___.___.___.___!   !___!
      2 | 1   1 | 3       1           1  
.___.___!   .   !___.___.   .___.   .   .
| 3   1   0   1   1     |   |   |        
!___.   .   .___.   .   !___!   !___.___.
  3 |     1 |   |             1   2   2 |
.___!   .   !   !___.___.   .___.___.   !
|           |     1     |   |       |   |
!   .   .   !___.   .   !___!   .   !___!
| 2   1   1     |         1   0   0      
!___.___.___.___!   .   .   .   .   .   .
```

## How To Use
```
put your input data into testcase.txt file
> ./a.out < testcase.txt
```