#include "FileManager.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/*
Tworzy pusty plik o nazwie 'filename' w katalogu "Resources" i otwiera go do zapisu.
Zwraca deskryptor otwartego pliku, gdy się udało; -1 gdy błąd.
*/
int FileManager::createFile(std::string filename) {
	std::string filepath = RESOURCES_DIR + filename;
	//tworz i ewentualnie kasuj zawartosc pliku, pisz zawsze na jego koncu, perms: 644):
	int fd = open(filepath.c_str(), O_CREAT|O_WRONLY|O_TRUNC|O_APPEND, S_IRUSR+S_IWUSR+S_IRGRP+S_IROTH);
	return fd;
}

/*
Sprawdza czy plik o nazwie 'filename' istnieje w katalogu "Resources".
Zwraca true, gdy istnieje; false w p.p.
*/
bool FileManager::checkFile(std::string filename) {
	int fd = openFile(filename, READ_F);
	if (fd < 0)
		return false;
	if (close(fd) != 0)
		return false; // ???
	return true;
}

/*
Otwiera plik o nazwie 'filename' z katalogu "Resources" z trybem określonym przez 'flag' (czytanie lub pisanie).
Zwraca deskryptor otwartego pliku, gdy udało się; -1 gdy błąd.
*/
int FileManager::openFile(std::string filename, int flag) {
	std::string filepath = RESOURCES_DIR + filename;
	int fd;
	if (flag == READ_F)
		fd = open(filepath.c_str(), O_RDONLY);
	else if (flag == WRITE_F)
		fd = open(filepath.c_str(), O_WRONLY);
	else
		return -1;
	return fd;
}

/*
Zamyka plik o deksryptorze 'fd'.
Zwraca true gdy się udało; false w p.p.
*/
bool FileManager::closeFile(int fd) {
	int result = close(fd);
	if (result != 0)
		return false;
	return true;
}

/*
Czyta 'buflen' bajtów z pliku o deskryptorze 'fd' do bufora 'buf'.
Zwraca:
- liczbę odczytanych bajtów
- 0 gdy EOF
- -1 gdy nastąpi błąd inny od EBADF i EINVAL (patrz: errno)
- -2 gdy nastąpi błąd EBADF lub EINVAL (nieprawidłowy deskryptor lub plik nie jest otwarty do czytania)
*/
size_t FileManager::readFile(int fd, void *buf, size_t buflen) {
	size_t bytesRead = read(fd, buf, buflen);
	if (bytesRead < 0) {
		if (errno == EBADF || errno == EINVAL)
			return -2;
		else
			return -1;
	}
	return bytesRead;
}

/*
Dopisuje 'buflen' bajtów do pliku o deskryptorze 'fd' z bufora 'buf'.
Zwraca:
- liczbę zapisanych bajtów
- -1 gdy nastąpi błąd inny od EBADF i EINVAL (patrz: errno)
- -2 gdy nastąpi błąd EBADF lub EINVAL (nieprawidłowy deskryptor lub plik nie jest otwarty do pisania)
- -3 gdy nastąpi błąd ENOSPC, EFBIG lub EDQUOT (brak miejsca na dane, etc.)
*/
size_t FileManager::appendFile(int fd, void *buf, size_t buflen) {
	size_t bytesWritten = write(fd, buf, buflen);
	if (bytesWritten < 0) {
		if (errno == EBADF || errno == EINVAL)
			return -2;
		else if (errno == ENOSPC || errno == EFBIG || errno == EDQUOT)
			return -3;
		else
			return -1;
	}
	return bytesWritten;
}

/*
Odczytuje wielkosc pliku w bajtach o deskryptorze 'fd'.
Zwraca:
- liczbę zapisanych bajtów
- -1 gdy nastąpi błąd inny od EBADF i EINVAL (patrz: errno)
- -2 gdy nastąpi błąd EBADF lub EINVAL (nieprawidłowy deskryptor lub plik nie jest otwarty do pisania)
*/
int FileManager::getFileSize(int fd) {
	int bytes = lseek(fd, 0, SEEK_END);
	if (bytes < 0) {
		if (errno == EBADF || errno == EINVAL)
			return -2;
		else
			return -1
	}
	return bytes;
}
