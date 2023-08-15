a:b
	gcc main.c -L "./mysql-8.1.0-winx64/lib" -llibmysql -o main.exe
b:
	gcc -c mysql.c -I "./mysql-8.1.0-winx64/include"