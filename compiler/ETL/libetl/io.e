#ifndef _IO_H_
#define _IO_H_
define @file_open($src, $op);
define file_print(@file, $data);
define file_write(@file, @data, size);
define file_read(@file, @data, size);
define file_seek(@file, offset, origin);
define file_close(@file);
define file_size(@file);
#endif
 