-- tde_extension--1.0.sql
CREATE FUNCTION tde_encrypt(text) RETURNS bytea
    AS '$libdir/tde_extension', 'tde_encrypt'
    LANGUAGE C STRICT;

CREATE FUNCTION tde_decrypt(bytea) RETURNS text
    AS '$libdir/tde_extension', 'tde_decrypt'
    LANGUAGE C STRICT;
