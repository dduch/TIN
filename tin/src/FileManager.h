#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include <string>

// tryby otwarcia pliku:
#define READ_F 0	// tylko odczyt
#define WRITE_F 1	// tylko zapis
// READ_WRITE nie bedzie u nas potrzebny, prawda?


namespace FileManager {

	const std::string RESOURCES_DIR = "Resources/";				// katalog z zasobami

	int createFile(std::string filename);						// tworzenie pliku w RESOURCE_DIR do zapisu
	bool checkFile(std::string filename);						// sprawdzanie czy plik istnieje w RESOURCE_DIR
	int openFile(std::string filename, int flag);				// otwarcie pliku w trybie czytania lub pisania w RESCOURCE_DIR
	bool closeFile(int fd);										// zamkniecie otwartego pliku
	size_t readFile(int fd, void *buf, size_t buflen);			// czytanie z otwartego pliku
	size_t appendFile(int fd, void *buf, size_t buflen);		// pisanie do otwartego pliku
	int getFileSize(std::string filename);						// odczytanie wielkosci pliku w bajtach
	int unlinkFile(std::string filename);						// usunięcie pliku z katalogu Resources
	int renameFile(std::string filename);	// zmienia nazwe pliku tymczasowego po zakonczeniu pobierania
};

#endif
