-- 1. (Re)Create the audio_files table
DROP TABLE IF EXISTS audio_files;
CREATE TABLE audio_files (
    id serial PRIMARY KEY,
    filename text,
    enc_audio_data bytea
);

-- 2. Insert audio files using the tde_encrypt function.
--    The file paths below must be accessible by the PostgreSQL server.
INSERT INTO audio_files (filename, enc_audio_data)
VALUES 
  ('audio1.mp3', tde_encrypt(pg_read_binary_file('C:\\Users\\musab\\Desktop\\tde\\1\\audio\\audio1.mp3'))),
  ('audio2.mp3', tde_encrypt(pg_read_binary_file('C:\\Users\\musab\\Desktop\\tde\\1\\audio\\audio2.mp3'))),
  ('audio3.mp3', tde_encrypt(pg_read_binary_file('C:\\Users\\musab\\Desktop\\tde\\1\\audio\\audio3.mp3')));

-- 3. Inspect the table: List each file with the length of the encrypted data.
SELECT 
  id, 
  filename, 
  length(enc_audio_data) AS encrypted_length
FROM audio_files;

-- 4. View the full encrypted data in hexadecimal format.
--    (pgAdmin will show a long hex string for each row.)
SELECT 
  id, 
  filename, 
  encode(enc_audio_data, 'hex') AS encrypted_hex
FROM audio_files;

-- 5. View the decrypted data’s length.
--    (tde_decrypt returns a bytea; its length should match the original file size.)
SELECT 
  id, 
  filename, 
  length(tde_decrypt(enc_audio_data)) AS decrypted_length
FROM audio_files;

-- 6. View the decrypted data in full hexadecimal format.
SELECT 
  id, 
  filename, 
  encode(tde_decrypt(enc_audio_data), 'hex') AS decrypted_hex
FROM audio_files;

-- 7. View only the first 100 characters of the decrypted data (hex format).
SELECT 
  id, 
  filename, 
  substring(encode(tde_decrypt(enc_audio_data), 'hex') FROM 1 FOR 100) AS decrypted_hex_snippet
FROM audio_files;
