-- 1. (Re)Create the image_files table
DROP TABLE IF EXISTS image_files;
CREATE TABLE image_files (
    id serial PRIMARY KEY,
    filename text,
    enc_image_data bytea
);

-- 2. Insert image files using the tde_encrypt function.
--    The file paths below must be accessible by the PostgreSQL server.
INSERT INTO image_files (filename, enc_image_data)
VALUES 
  ('image1.jpg', tde_encrypt(pg_read_binary_file('C:\\Users\\musab\\Desktop\\tde\\1\\images\\fruits.jpg'))),
  ('image2.png', tde_encrypt(pg_read_binary_file('C:\\Users\\musab\\Desktop\\tde\\1\\images\\coat.jpg')));

-- 3. Inspect the table: List each file with the length of the encrypted data.
SELECT 
  id, 
  filename, 
  length(enc_image_data) AS encrypted_length
FROM image_files;

-- 4. View the full encrypted data in hexadecimal format.
SELECT 
  id, 
  filename, 
  encode(enc_image_data, 'hex') AS encrypted_hex
FROM image_files;

-- 5. View the decrypted data’s length.
SELECT 
  id, 
  filename, 
  length(tde_decrypt(enc_image_data)) AS decrypted_length
FROM image_files;

-- 6. View the decrypted data in full hexadecimal format.
SELECT 
  id, 
  filename, 
  encode(tde_decrypt(enc_image_data), 'hex') AS decrypted_hex
FROM image_files;

-- 7. View only the first 100 characters of the decrypted data (hex format).
SELECT 
  id, 
  filename, 
  substring(encode(tde_decrypt(enc_image_data), 'hex') FROM 1 FOR 100) AS decrypted_hex_snippet
FROM image_files;
