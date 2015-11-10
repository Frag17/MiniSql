# MiniSql
GTMDSQL

This is a mini database system. You can build a simple database and manage it by MiniSql.
Supported operations and format. ([name]--you should set a name yourself. ?--have it or don't have. /--choose one.)
create a table, format:
    create table [tablename] (
        [attributename]  char([n])/int/float  ?unique ,
        [attributename]  char([n])/int/float  ?unique ,
        ...
        primary key([attributename])
    );
insert values into a table, format:
    insert into [tablename] values([value1],[value2], ...);
select values with condition, format:
    select * from [tablename];
    select * from [tablename] where [attributename] =/<>/</>/<=/>= [value];
    select * from [tablename] where [attributename1] =/<>/</>/<=/>= [value1] and [attributename2] =/<>/</>/<=/>= [value2];
delete values withc condition, format:
    delete from [tablename] where [attributename] =/<>/</>/<=/>= [value];
create index on a unique attribute, format:
    create index [indexname] on [tablename]([attributename]);
drop table or drop index, format:
    drop table/index [name];
PS: ';' is necessary at the end of each query.

supported data type:
int, float, char(n) 1<=n<=255

multiple data base is not supported.

source code can only be bulit in Vusial Studio 2005 or higher version.

Shuke Peng, Wenqi Fu and Dongyu Zhong reserve the copyright of source code.

