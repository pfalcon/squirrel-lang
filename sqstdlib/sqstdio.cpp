/* see copyright notice in squirrel.h */
#include <new>
#include <stdio.h>
#include <string.h>
#include <squirrel.h>
#include <sqstdio.h>
#include "sqstdstream.h"
#ifdef SQPHYSFS
#include <physfs.h>
#include <wchar.h>
#endif //SQPHYSFS

#define SQSTD_FILE_TYPE_TAG (SQSTD_STREAM_TYPE_TAG | 0x00000001)
//basic API
SQFILE sqstd_fopen(const SQChar *filename ,const SQChar *mode)
{
#ifndef SQUNICODE
#ifndef SQPHYSFS
	return (SQFILE)fopen(filename,mode);
#else

	SQFILE f;
	if( strcmp( mode,"r")==0 || strcmp( mode,"rb")==0 ) {
		f = (SQFILE) PHYSFS_openRead( filename );
	} else if( strcmp( mode,"w")==0 || strcmp( mode,"wb")==0 ) {
		f = (SQFILE) PHYSFS_openWrite( filename );
	} else if( strcmp( mode,"a")==0 || strcmp( mode,"ab")==0 ) {
		f = (SQFILE) PHYSFS_openAppend( filename );
	} else {
		f = NULL;
	}
	return f;
#endif //SQPHYSFS
#else
#ifndef SQPHYSFS
	return (SQFILE)_wfopen(filename,mode);
#else
	int len = wcslen(filename);
	char *buff = new char[ ( len+1)*sizeof(wchar_t) ];
	if( sizeof(wchar_t)==2 ){
		PHYSFS_utf8FromUcs2( (PHYSFS_uint16*) filename, buff, len+1 );
	}else if( sizeof(wchar_t)==4 ){ 
		PHYSFS_utf8FromUcs4( (PHYSFS_uint32*) filename, buff, len+1 );
	}

	SQFILE f;
	
	if( wcscmp( mode,L"r")==0 || wcscmp( mode,L"rb")==0 ) {
		f = (SQFILE) PHYSFS_openRead( buff );
	} else if( wcscmp( mode,L"w")==0 || wcscmp( mode,L"wb")==0 ) {
		f = (SQFILE) PHYSFS_openWrite( buff );
	} else if( wcscmp( mode,L"a")==0 || wcscmp( mode,L"ab")==0 ) {
		f = (SQFILE) PHYSFS_openAppend( buff );
	} else {
		f = NULL;
	}
	delete[] buff;
	return f;
#endif //SQPHYSFS
#endif //SQUNICODE
}

SQInteger sqstd_fread(void* buffer, SQInteger size, SQInteger count, SQFILE file)
{
#ifndef SQPHYSFS
	SQInteger ret = (SQInteger)fread(buffer,size,count,(FILE *)file);
	return ret;
#else
	SQInteger ret = PHYSFS_read( (PHYSFS_File*) file, buffer, size, count );
	return ret;
#endif //SQPHYSFS
}

SQInteger sqstd_flen(SQFILE file)
{
#ifndef SQPHYSFS
	SQInteger prevpos=sqstd_ftell(file);
	sqstd_fseek(file,0,SQ_SEEK_END);
	SQInteger size=sqstd_ftell(file);
	sqstd_fseek(file,prevpos,SQ_SEEK_SET);
	return size;
#else
	return PHYSFS_fileLength( (PHYSFS_File*) file );
#endif //SQPHYSFS
}

SQInteger sqstd_fgets(void* buffer, SQInteger size, SQFILE file)
{
#ifndef SQPHYSFS
	char *p = fgets((char*)buffer,size,(FILE *)file);
	if (!p)
		return 0;
	return (SQInteger)strlen(p);
#else
	char *p = (char*) buffer;
	if( sqstd_feof( file ) || size<=0 || sqstd_ftell(file)==sqstd_flen(file) ) {
		return 0;
	}
	SQInteger len=0;
	int run = 1;
	while( run && len!=size ){
		run = 0;

		run = sqstd_fread( p, 1, 1, file );
		if( (*p)=='\n' ){
			run = 0;
		}
		p++;
		len++;
	}
	return len;
#endif //SQPHYSFS
}

SQInteger sqstd_fwrite(const SQUserPointer buffer, SQInteger size, SQInteger count, SQFILE file)
{
#ifndef SQPHYSFS
	return (SQInteger)fwrite(buffer,size,count,(FILE *)file);
#else
	return (SQInteger) PHYSFS_write( (PHYSFS_File*) file, buffer, size, count );
#endif //SQPHYSFS
}

SQInteger sqstd_fseek(SQFILE file, SQInteger offset, SQInteger origin)
{
#ifndef SQPHYSFS
	SQInteger realorigin;
	switch(origin) {
		case SQ_SEEK_CUR: realorigin = SEEK_CUR; break;
		case SQ_SEEK_END: realorigin = SEEK_END; break;
		case SQ_SEEK_SET: realorigin = SEEK_SET; break;
		default: return -1; //failed
	}
	return fseek((FILE *)file,(long)offset,(int)realorigin);
#else
	PHYSFS_File *handle = (PHYSFS_File *) file;
	PHYSFS_sint64 pos = 0, len, current;

	switch( origin ) {
		case SQ_SEEK_SET:
			pos = offset;
			break;
		case SQ_SEEK_CUR:
			current = PHYSFS_tell(handle);
			if (current == -1) {
				return -1;
			}
			pos = current;
			if (offset == 0)
				return 0;
			pos += offset;
			break;
		case SQ_SEEK_END:
			len = PHYSFS_fileLength(handle);
			if (len == -1) {
				return -1;
			}
			pos = len;
			pos += offset;
			break;
		default:
			return -1; //failed
	}

	if ( pos < 0 ) {
		return -1;
	}
	if (!PHYSFS_seek(handle, pos)) {
		return -1;
	}
	return 0;
#endif
}

SQInteger sqstd_ftell(SQFILE file)
{
#ifndef SQPHYSFS
	return ftell((FILE *)file);
#else
	return PHYSFS_tell( (PHYSFS_File*) file );
#endif //SQPHYSFS
}

SQInteger sqstd_fflush(SQFILE file)
{
#ifndef SQPHYSFS
	return fflush((FILE *)file);
#else
	return PHYSFS_flush( (PHYSFS_File*) file );
#endif //SQPHYSFS
}

SQInteger sqstd_fclose(SQFILE file)
{
#ifndef SQPHYSFS
	return fclose((FILE *)file);
#else
	return PHYSFS_close( (PHYSFS_File*) file );
#endif //SQPHYSFS
}

SQInteger sqstd_feof(SQFILE file)
{
#ifndef SQPHYSFS
	return feof((FILE *)file);
#else
	return PHYSFS_eof((PHYSFS_File *)file);
#endif //SQPHYSFS
}

//File
struct SQFile : public SQStream {
	SQFile() { _handle = NULL; _owns = false;}
	SQFile(SQFILE file, bool owns) { _handle = file; _owns = owns;}
	virtual ~SQFile() { Close(); }
	bool Open(const SQChar *filename ,const SQChar *mode) {
		Close();
		if( (_handle = sqstd_fopen(filename,mode)) ) {
			_owns = true;
			return true;
		}
		return false;
	}
	void Close() {
		if(_handle && _owns) { 
			sqstd_fclose(_handle);
			_handle = NULL;
			_owns = false;
		}
	}
	SQInteger Read(void *buffer,SQInteger size) {
		return sqstd_fread(buffer,1,size,_handle);
	}
	SQInteger Readline(void *buffer,SQInteger size) {
		return sqstd_fgets(buffer,size,_handle);
	}
	SQInteger Write(void *buffer,SQInteger size) {
		return sqstd_fwrite(buffer,1,size,_handle);
	}
	SQInteger Flush() {
		return sqstd_fflush(_handle);
	}
	SQInteger Tell() {
		return sqstd_ftell(_handle);
	}
	SQInteger Len() {
		return sqstd_flen(_handle);
	}

	SQInteger Seek(SQInteger offset, SQInteger origin)	{
		return sqstd_fseek(_handle,offset,origin);
	}
	bool IsValid() { return _handle?true:false; }
	bool EOS() { return Tell()==Len()?true:false;}
	SQFILE GetHandle() {return _handle;}
private:
	SQFILE _handle;
	bool _owns;
};

static SQInteger _file__typeof(HSQUIRRELVM v)
{
	sq_pushstring(v,_SC("file"),-1);
	return 1;
}

static SQInteger _file_releasehook(SQUserPointer p, SQInteger size)
{
	SQFile *self = (SQFile*)p;
	self->~SQFile();
	sq_free(self,sizeof(SQFile));
	return 1;
}

static SQInteger _file_constructor(HSQUIRRELVM v)
{
	const SQChar *filename,*mode;
	bool owns = true;
	SQFile *f;
	SQFILE newf;
	if(sq_gettype(v,2) == OT_STRING && sq_gettype(v,3) == OT_STRING) {
		sq_getstring(v, 2, &filename);
		sq_getstring(v, 3, &mode);
		newf = sqstd_fopen(filename, mode);
		if(!newf) return sq_throwerror(v, _SC("cannot open file"));
	} else if(sq_gettype(v,2) == OT_USERPOINTER) {
		owns = !(sq_gettype(v,3) == OT_NULL);
		sq_getuserpointer(v,2,&newf);
	} else {
		return sq_throwerror(v,_SC("wrong parameter"));
	}
	
	f = new (sq_malloc(sizeof(SQFile)))SQFile(newf,owns);
	if(SQ_FAILED(sq_setinstanceup(v,1,f))) {
		f->~SQFile();
		sq_free(f,sizeof(SQFile));
		return sq_throwerror(v, _SC("cannot create blob with negative size"));
	}
	sq_setreleasehook(v,1,_file_releasehook);
	return 0;
}

static SQInteger _file_close(HSQUIRRELVM v)
{
	SQFile *self = NULL;
	if(SQ_SUCCEEDED(sq_getinstanceup(v,1,(SQUserPointer*)&self,(SQUserPointer)SQSTD_FILE_TYPE_TAG))
		&& self != NULL)
	{
		self->Close();
	}
	return 0;
}

//bindings
#define _DECL_FILE_FUNC(name,nparams,typecheck) {_SC(#name),_file_##name,nparams,typecheck}
static SQRegFunction _file_methods[] = {
	_DECL_FILE_FUNC(constructor,3,_SC("x")),
	_DECL_FILE_FUNC(_typeof,1,_SC("x")),
	_DECL_FILE_FUNC(close,1,_SC("x")),
	{0,0,0,0},
};



SQRESULT sqstd_createfile(HSQUIRRELVM v, SQFILE file,SQBool own)
{
	SQInteger top = sq_gettop(v);
	sq_pushregistrytable(v);
	sq_pushstring(v,_SC("std_file"),-1);
	if(SQ_SUCCEEDED(sq_get(v,-2))) {
		sq_remove(v,-2); //removes the registry
		sq_pushroottable(v); // push the this
		sq_pushuserpointer(v,file); //file
		if(own){
			sq_pushinteger(v,1); //true
		}
		else{
			sq_pushnull(v); //false
		}
		if(SQ_SUCCEEDED( sq_call(v,3,SQTrue,SQFalse) )) {
			sq_remove(v,-2);
			return SQ_OK;
		}
	}
	sq_settop(v,top);
	return SQ_ERROR;
}

SQRESULT sqstd_getfile(HSQUIRRELVM v, SQInteger idx, SQFILE *file)
{
	SQFile *fileobj = NULL;
	if(SQ_SUCCEEDED(sq_getinstanceup(v,idx,(SQUserPointer*)&fileobj,(SQUserPointer)SQSTD_FILE_TYPE_TAG))) {
		*file = fileobj->GetHandle();
		return SQ_OK;
	}
	return sq_throwerror(v,_SC("not a file"));
}



#define IO_BUFFER_SIZE 2048
struct IOBuffer {
	unsigned char buffer[IO_BUFFER_SIZE];
	int size;
	int ptr;
	SQFILE file;
};

SQInteger _read_byte(IOBuffer *iobuffer)
{
	if(iobuffer->ptr < iobuffer->size) {

		SQInteger ret = iobuffer->buffer[iobuffer->ptr];
		iobuffer->ptr++;
		return ret;
	}
	else {
		if( (iobuffer->size = sqstd_fread(iobuffer->buffer,1,IO_BUFFER_SIZE,iobuffer->file )) > 0 )
		{
			SQInteger ret = iobuffer->buffer[0];
			iobuffer->ptr = 1;
			return ret;
		}
	}

	return 0;
}

SQInteger _read_two_bytes(IOBuffer *iobuffer)
{
	if(iobuffer->ptr < iobuffer->size) {
		if(iobuffer->size < 2) return 0;
		SQInteger ret = *((wchar_t*)&iobuffer->buffer[iobuffer->ptr]);
		iobuffer->ptr += 2;
		return ret;
	}
	else {
		if( (iobuffer->size = sqstd_fread(iobuffer->buffer,1,IO_BUFFER_SIZE,iobuffer->file )) > 0 )
		{
			if(iobuffer->size < 2) return 0;
			SQInteger ret = *((wchar_t*)&iobuffer->buffer[0]);
			iobuffer->ptr = 2;
			return ret;
		}
	}

	return 0;
}

static SQInteger _io_file_lexfeed_PLAIN(SQUserPointer iobuf)
{
	IOBuffer *iobuffer = (IOBuffer *)iobuf;
	return _read_byte(iobuffer);

}

#ifdef SQUNICODE
static SQInteger _io_file_lexfeed_UTF8(SQUserPointer iobuf)
{
	IOBuffer *iobuffer = (IOBuffer *)iobuf;
#define READ(iobuf) \
	if((inchar = (unsigned char)_read_byte(iobuf)) == 0) \
		return 0;

	static const SQInteger utf8_lengths[16] =
	{
		1,1,1,1,1,1,1,1,        /* 0000 to 0111 : 1 byte (plain ASCII) */
		0,0,0,0,                /* 1000 to 1011 : not valid */
		2,2,                    /* 1100, 1101 : 2 bytes */
		3,                      /* 1110 : 3 bytes */
		4                       /* 1111 :4 bytes */
	};
	static unsigned char byte_masks[5] = {0,0,0x1f,0x0f,0x07};
	unsigned char inchar;
	SQInteger c = 0;
	READ(iobuffer);
	c = inchar;
	//
	if(c >= 0x80) {
		SQInteger tmp;
		SQInteger codelen = utf8_lengths[c>>4];
		if(codelen == 0) 
			return 0;
			//"invalid UTF-8 stream";
		tmp = c&byte_masks[codelen];
		for(SQInteger n = 0; n < codelen-1; n++) {
			tmp<<=6;
			READ(iobuffer);
			tmp |= inchar & 0x3F;
		}
		c = tmp;
	}
	return c;
}
#endif

static SQInteger _io_file_lexfeed_UCS2_LE(SQUserPointer iobuf)
{
	SQInteger ret;
	IOBuffer *iobuffer = (IOBuffer *)iobuf;
	if( (ret = _read_two_bytes(iobuffer)) > 0 )
		return ret;
	return 0;
}

static SQInteger _io_file_lexfeed_UCS2_BE(SQUserPointer iobuf)
{
	SQInteger c;
	IOBuffer *iobuffer = (IOBuffer *)iobuf;
	if( (c = _read_two_bytes(iobuffer)) > 0 ) {
		c = ((c>>8)&0x00FF)| ((c<<8)&0xFF00);
		return c;
	}
	return 0;
}

SQInteger file_read(SQUserPointer file,SQUserPointer buf,SQInteger size)
{
	SQInteger ret;
	if( ( ret = sqstd_fread(buf,1,size,(SQFILE)file ))!=0 )return ret;
	return -1;
}

SQInteger file_write(SQUserPointer file,SQUserPointer p,SQInteger size)
{
	return sqstd_fwrite(p,1,size,(SQFILE)file);
}

SQRESULT sqstd_loadfile(HSQUIRRELVM v,const SQChar *filename,SQBool printerror)
{
	SQFILE file = sqstd_fopen(filename,_SC("rb"));
	
	SQInteger ret;
	unsigned short us;
	unsigned char uc;
	SQLEXREADFUNC func = _io_file_lexfeed_PLAIN;
	if(file){
		ret = sqstd_fread(&us,1,2,file);
		if(ret != 2) {
			//probably an empty file
			us = 0;
		}
		if(us == SQ_BYTECODE_STREAM_TAG) { //BYTECODE
			sqstd_fseek(file,0,SQ_SEEK_SET);
			if(SQ_SUCCEEDED(sq_readclosure(v,file_read,file))) {
				sqstd_fclose(file);
				return SQ_OK;
			}
		}
		else { //SCRIPT
			
			switch(us)
			{
				//gotta swap the next 2 lines on BIG endian machines
				case 0xFFFE: func = _io_file_lexfeed_UCS2_BE; break;//UTF-16 little endian;
				case 0xFEFF: func = _io_file_lexfeed_UCS2_LE; break;//UTF-16 big endian;
				case 0xBBEF: 
					if(sqstd_fread(&uc,1,sizeof(uc),file) == 0) { 
						sqstd_fclose(file); 
						return sq_throwerror(v,_SC("io error")); 
					}
					if(uc != 0xBF) { 
						sqstd_fclose(file); 
						return sq_throwerror(v,_SC("Unrecognozed ecoding")); 
					}
#ifdef SQUNICODE
					func = _io_file_lexfeed_UTF8;
#else
					func = _io_file_lexfeed_PLAIN;
#endif
					break;//UTF-8 ;
				default: sqstd_fseek(file,0,SQ_SEEK_SET); break; // ascii
			}
			IOBuffer buffer;
			buffer.ptr = 0;
			buffer.size = 0;
			buffer.file = file;
			if(SQ_SUCCEEDED(sq_compile(v,func,&buffer,filename,printerror))){
				sqstd_fclose(file);
				return SQ_OK;
			}
		}
		sqstd_fclose(file);
		return SQ_ERROR;
	}
	return sq_throwerror(v,_SC("cannot open the file"));
}

SQRESULT sqstd_dofile(HSQUIRRELVM v,const SQChar *filename,SQBool retval,SQBool printerror)
{
	if(SQ_SUCCEEDED(sqstd_loadfile(v,filename,printerror))) {
		sq_push(v,-2);
		if(SQ_SUCCEEDED(sq_call(v,1,retval,SQTrue))) {
			sq_remove(v,retval?-2:-1); //removes the closure
			return 1;
		}
		sq_pop(v,1); //removes the closure
	}
	return SQ_ERROR;
}

SQRESULT sqstd_writeclosuretofile(HSQUIRRELVM v,const SQChar *filename)
{
	SQFILE file = sqstd_fopen(filename,_SC("wb"));
	if(!file) return sq_throwerror(v,_SC("cannot open the file"));
	if(SQ_SUCCEEDED(sq_writeclosure(v,file_write,file))) {
		sqstd_fclose(file);
		return SQ_OK;
	}
	sqstd_fclose(file);
	return SQ_ERROR; //forward the error
}

SQInteger _g_io_loadfile(HSQUIRRELVM v)
{
	const SQChar *filename;
	SQBool printerror = SQFalse;
	sq_getstring(v,2,&filename);
	if(sq_gettop(v) >= 3) {
		sq_getbool(v,3,&printerror);
	}
	if(SQ_SUCCEEDED(sqstd_loadfile(v,filename,printerror)))
		return 1;
	return SQ_ERROR; //propagates the error
}

SQInteger _g_io_writeclosuretofile(HSQUIRRELVM v)
{
	const SQChar *filename;
	sq_getstring(v,2,&filename);
	if(SQ_SUCCEEDED(sqstd_writeclosuretofile(v,filename)))
		return 1;
	return SQ_ERROR; //propagates the error
}

SQInteger _g_io_dofile(HSQUIRRELVM v)
{
	const SQChar *filename;
	SQBool printerror = SQFalse;
	sq_getstring(v,2,&filename);
	if(sq_gettop(v) >= 3) {
		sq_getbool(v,3,&printerror);
	}
	sq_push(v,1); //repush the this
	if(SQ_SUCCEEDED(sqstd_dofile(v,filename,SQTrue,printerror)))
		return 1;
	return SQ_ERROR; //propagates the error
}

#define _DECL_GLOBALIO_FUNC(name,nparams,typecheck) {_SC(#name),_g_io_##name,nparams,typecheck}
static SQRegFunction iolib_funcs[]={
	_DECL_GLOBALIO_FUNC(loadfile,-2,_SC(".sb")),
	_DECL_GLOBALIO_FUNC(dofile,-2,_SC(".sb")),
	_DECL_GLOBALIO_FUNC(writeclosuretofile,3,_SC(".sc")),
	{0,0}
};

SQRESULT sqstd_register_iolib(HSQUIRRELVM v)
{
	SQInteger top = sq_gettop(v);
	//create delegate
	declare_stream(v,_SC("file"),(SQUserPointer)SQSTD_FILE_TYPE_TAG,_SC("std_file"),_file_methods,iolib_funcs);
	sq_pushstring(v,_SC("stdout"),-1);
	sqstd_createfile(v,stdout,SQFalse);
	sq_newslot(v,-3,SQFalse);
	sq_pushstring(v,_SC("stdin"),-1);
	sqstd_createfile(v,stdin,SQFalse);
	sq_newslot(v,-3,SQFalse);
	sq_pushstring(v,_SC("stderr"),-1);
	sqstd_createfile(v,stderr,SQFalse);
	sq_newslot(v,-3,SQFalse);
	sq_settop(v,top);
	return SQ_OK;
}
