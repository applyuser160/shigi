a:b
	gcc main.c -L "./mysql-8.0.33-winx64/lib" -llibmysql -o main.exe
b:
	gcc -c mysql.c -I "./mysql-8.0.33-winx64/include"