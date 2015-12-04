
#ifndef DIRECTORY_HPP
#define DIRECTORY_HPP
#include<vector>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<iostream>
#include <sys/types.h>
#include <sys/dir.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

using namespace std;

/* Given the name of a directory, this program prints a list of files in that directory. If no
 * argument is provided, it prints the entries in the directory in which the program runs.

 To compile: g++ -std=c++11 printDirectoryEntries.cpp
 To run: ./a.out name-of-a-directory

 Written by: Ali A. Kooshesh

 */

class DirEntry {
// DirEntry represents a directory entry that is not a directory. For example, an ASCII file, a
// binary file, etc.

public:
    DirEntry( const char *eName, struct stat &s, const char *_pparent ) {
        _name = new char[ strlen( eName ) + 1 ];
        strcpy( _name, eName );
        pathToParent = _pparent;  // do not delete this.
        st_mode = s.st_mode;
        byteCount = s.st_size;
        iNode = s.st_ino;
    }
    virtual ~DirEntry()       { delete [] _name; }  // nothing to delete!
    const char *name()        { return _name; }
    const char *path()  const { return pathToParent;    }  
    bool isRegularFile()      { return st_mode & S_IFREG;   }
    bool isDirectoryFile()    { return st_mode & S_IFDIR;  }
    off_t numBytes()          { return byteCount; }
    ino_t inode()             { return iNode; }

    virtual void print() {
        std::cout << "Entry-name: "      << path() << "/" << name() << std::endl;
        //std::cout <<  ":\t A regular-file";
        //std::cout << "\t i-node: "  << inode() << std::endl;
    }

private:
    char* _name;
    mode_t st_mode;
    ino_t iNode;
    off_t byteCount;
    const char *pathToParent;
};

class Directory: public DirEntry {
public:
    Directory( const char *fName, struct stat s, const char *path ): DirEntry( fName, s, path )   {}
    ~Directory() {}
    void print() {
        std::cout << "Entry-name: "      << path() << "/" << name() << std::endl;
       // std::cout <<  ":\t A directory-file";
       // std::cout << "\t i-node: "  << inode() << std::endl;
    }

    bool &visible( const char *path )   { return _visible;        }

private:
    bool _visible;
};

class DirectoryListing {
// Represents a single directory entry.

public:
    ~DirectoryListing() {
        for( vector<DirEntry *>::iterator iter = dirListing.begin(); iter != dirListing.end(); ++iter )
            delete *iter ;
    }
        
    void addEntry( DirEntry * ent )   {
        dirListing.push_back( ent );
    }
    void addEntry( const char *fName, struct stat &s, const char *path ) { 
        dirListing.push_back( s.st_mode & S_IFDIR ? new Directory(fName, s, path) : new  DirEntry(fName, s, path) ); 
    }
    DirEntry *getEntry( unsigned int idx ) { return dirListing[ idx ]; }  // trouble! SegFault likely
    int numEntries() { return dirListing.size(); }
    void print() {
        for( vector< DirEntry * >::iterator iter = dirListing.begin(); iter != dirListing.end(); ++iter )
            (*iter)->print();
    }

private:
    vector < DirEntry * > dirListing;
};

/*int main( int argc, char *argv[] ) {
    const char *dirName = ".";
    if( argc == 2 )
        dirName = argv[1];
    // DIR is a typedef defined in /usr/include/dirent.h. It is a struct 
    // that represents a directory stream object.
    DIR *curDir = opendir( dirName );
    if( curDir == NULL ) {
        cout << "Unable to open " << dirName << ": " << strerror( errno ) << endl;
        exit( -1 );
    }
    struct dirent *dEntry;
    DirectoryListing dirL;
    while( ( dEntry = readdir( curDir ) ) != NULL ) {
        struct stat dStat;
        if( stat( dEntry->d_name, &dStat ) == -1 ) {
            //cout << "Unable to get stat for directory-entry " << dEntry->d_name;
          //  cout << strerror( errno ) << endl;
            //exit( -1 );
        }
	if (dEntry->d_name[0] != '.')
		dirL.addEntry( dEntry->d_name, dStat, dirName );
    }
    std::cout << "There are " << dirL.numEntries() << " entries in this directory.\n";
    dirL.print();
}
*/
#endif
