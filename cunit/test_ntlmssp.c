
#include <stdio.h>
#include <stdlib.h>
#include <freerdp/freerdp.h>
#include "ntlmssp.h"
#include "test_ntlmssp.h"

int init_ntlmssp_suite(void)
{
	return 0;
}

int clean_ntlmssp_suite(void)
{
	return 0;
}

int add_ntlmssp_suite(void)
{
	add_test_suite(ntlmssp);

	add_test_function(ntlmssp_compute_lm_hash);
	add_test_function(ntlmssp_compute_ntlm_hash);
	add_test_function(ntlmssp_compute_ntlm_v2_hash);
	add_test_function(ntlmssp_compute_lm_response);
	add_test_function(ntlmssp_compute_lm_v2_response);
	add_test_function(ntlmssp_compute_ntlm_v2_response);
	add_test_function(ntlmssp_generate_client_signing_key);
	add_test_function(ntlmssp_generate_server_signing_key);
	add_test_function(ntlmssp_generate_client_sealing_key);
	add_test_function(ntlmssp_generate_server_sealing_key);
	add_test_function(ntlmssp_encrypt_random_session_key);
	add_test_function(ntlmssp_encrypt_message);
	add_test_function(ntlmssp_decrypt_message);

	return 0;
}

void test_ntlmssp_compute_lm_hash(void)
{
	int i;
	int lm_hash_good;
	char lm_hash[16];
	char password[] = "Password";
	char expected_lm_hash[16] = "\xe5\x2c\xac\x67\x41\x9a\x9a\x22\x4a\x3b\x10\x8f\x3f\xa6\xcb\x6d";

	ntlmssp_compute_lm_hash(password, lm_hash);

	lm_hash_good = 1;
	for (i = 0; i < 16; i++) {
		if (lm_hash[i] != expected_lm_hash[i])
			lm_hash_good = 0;
	}

	CU_ASSERT(lm_hash_good == 1);
}

void test_ntlmssp_compute_ntlm_hash(void)
{
	int i;
	NTLMSSP *ntlmssp;
	int ntlm_hash_good;
	char ntlm_hash[16];
	char password[] = "Password";
	char expected_ntlm_hash[16] = "\xa4\xf4\x9c\x40\x65\x10\xbd\xca\xb6\x82\x4e\xe7\xc3\x0f\xd8\x52";

	ntlmssp = ntlmssp_new();
	ntlmssp_set_password(ntlmssp, password);

	ntlmssp_compute_ntlm_hash(&ntlmssp->password, ntlm_hash);

	ntlm_hash_good = 1;
	for (i = 0; i < 16; i++) {
		if (ntlm_hash[i] != expected_ntlm_hash[i])
			ntlm_hash_good = 0;
	}

	CU_ASSERT(ntlm_hash_good == 1);
}

void test_ntlmssp_compute_ntlm_v2_hash(void)
{
	int i;
	NTLMSSP *ntlmssp;
	int ntlm_v2_hash_good;
	char ntlm_v2_hash[16];

	char username[] = "User";
	char password[] = "Password";
	char domain[] = "Domain";
	char expected_ntlm_v2_hash[16] = "\x0c\x86\x8a\x40\x3b\xfd\x7a\x93\xa3\x00\x1e\xf2\x2e\xf0\x2e\x3f";

	ntlmssp = ntlmssp_new();
	ntlmssp_set_password(ntlmssp, password);
	ntlmssp_set_username(ntlmssp, username);
	ntlmssp_set_domain(ntlmssp, domain);

	ntlmssp_compute_ntlm_v2_hash(&ntlmssp->password, &ntlmssp->username, &ntlmssp->domain, ntlm_v2_hash);

	ntlm_v2_hash_good = 1;
	for (i = 0; i < 16; i++) {
		if (ntlm_v2_hash[i] != expected_ntlm_v2_hash[i])
			ntlm_v2_hash_good = 0;
	}

	CU_ASSERT(ntlm_v2_hash_good == 1);
}

void test_ntlmssp_compute_lm_response(void)
{
	int i;
	int lm_response_good;
	char lm_response[24];
	char password[] = "SecREt01";
	char challenge[] = "\x01\x23\x45\x67\x89\xAB\xCD\xEF";
	char expected_lm_response[24] = "\xC3\x37\xCD\x5C\xBD\x44\xFC\x97\x82\xA6\x67\xAF\x6D\x42\x7C\x6D\xE6\x7C\x20\xC2\xD3\xE7\x7C\x56";

	ntlmssp_compute_lm_response(password, challenge, lm_response);

	lm_response_good = 1;
	for (i = 0; i < 24; i++) {
		if (lm_response[i] != expected_lm_response[i])
			lm_response_good = 0;
	}

	CU_ASSERT(lm_response_good == 1);
}

void test_ntlmssp_compute_lm_v2_response(void)
{
	int i;
	char *p;
	NTLMSSP *ntlmssp;
	int lm_v2_response_good;
	char password[] = "password";
	char username[] = "username";
	char domain[] = "win7";
	char server_challenge[8] = "\x26\x6e\xcd\x75\xaa\x41\xe7\x6f";
	char lm_client_challenge[8] = "\x47\xa2\xe5\xcf\x27\xf7\x3c\x43";
	char expected_lm_v2_response[24] = "\xa0\x98\x01\x10\x19\xbb\x5d\x00\xf6\xbe\x00\x33\x90\x20\x34\xb3\x47\xa2\xe5\xcf\x27\xf7\x3c\x43";

	ntlmssp = ntlmssp_new();
	ntlmssp_set_password(ntlmssp, password);
	ntlmssp_set_username(ntlmssp, username);
	ntlmssp_set_domain(ntlmssp, domain);

	memcpy(ntlmssp->server_challenge, server_challenge, 8);
	memcpy(ntlmssp->client_challenge, lm_client_challenge, 8);

	ntlmssp_compute_lm_v2_response(ntlmssp);

	p = (char*) ntlmssp->lm_challenge_response.data;

	lm_v2_response_good = 1;
	for (i = 0; i < 24; i++) {
		if (p[i] != expected_lm_v2_response[i])
			lm_v2_response_good = 0;
	}

	CU_ASSERT(lm_v2_response_good == 1);
}

void test_ntlmssp_compute_ntlm_v2_response(void)
{
	int i;
	char* p;
	NTLMSSP *ntlmssp;
	int session_base_key_good;
	int lm_challenge_response_good;
	int nt_challenge_response_good;
	char password[] = "password";
	char username[] = "username";
	char domain[] = "win7";
	char timestamp[8] = "\xc3\x83\xa2\x1c\x6c\xb0\xcb\x01";
	char client_challenge[8] = "\x47\xa2\xe5\xcf\x27\xf7\x3c\x43";
	char server_challenge[8] = "\x26\x6e\xcd\x75\xaa\x41\xe7\x6f";

	char target_info_data[68] =
		"\x02\x00\x08\x00\x57\x00\x49\x00\x4e\x00\x37\x00"
		"\x01\x00\x08\x00\x57\x00\x49\x00\x4e\x00\x37\x00"
		"\x04\x00\x08\x00\x77\x00\x69\x00\x6e\x00\x37\x00"
		"\x03\x00\x08\x00\x77\x00\x69\x00\x6e\x00\x37\x00"
		"\x07\x00\x08\x00\xa9\x8d\x9b\x1a\x6c\xb0\xcb\x01"
		"\x00\x00\x00\x00\x00\x00\x00\x00";

	char expected_nt_challenge_response[112] =
		"\x01\x4a\xd0\x8c\x24\xb4\x90\x74\x39\x68\xe8\xbd\x0d\x2b\x70\x10"
		"\x01\x01\x00\x00\x00\x00\x00\x00\xc3\x83\xa2\x1c\x6c\xb0\xcb\x01"
		"\x47\xa2\xe5\xcf\x27\xf7\x3c\x43\x00\x00\x00\x00\x02\x00\x08\x00"
		"\x57\x00\x49\x00\x4e\x00\x37\x00\x01\x00\x08\x00\x57\x00\x49\x00"
		"\x4e\x00\x37\x00\x04\x00\x08\x00\x77\x00\x69\x00\x6e\x00\x37\x00"
		"\x03\x00\x08\x00\x77\x00\x69\x00\x6e\x00\x37\x00\x07\x00\x08\x00"
		"\xa9\x8d\x9b\x1a\x6c\xb0\xcb\x01\x00\x00\x00\x00\x00\x00\x00\x00";

	char expected_lm_challenge_response[24] =
		"\xa0\x98\x01\x10\x19\xbb\x5d\x00\xf6\xbe\x00\x33\x90\x20\x34\xb3"
		"\x47\xa2\xe5\xcf\x27\xf7\x3c\x43";

	char expected_session_base_key[16] =
		"\x6e\xf1\x6b\x79\x88\xf2\x3d\x7e\x54\x2a\x1a\x38\x4e\xa0\x6b\x52";

	ntlmssp = ntlmssp_new();
	ntlmssp_set_password(ntlmssp, password);
	ntlmssp_set_username(ntlmssp, username);
	ntlmssp_set_domain(ntlmssp, domain);

	memcpy(ntlmssp->timestamp, timestamp, 8);
	memcpy(ntlmssp->server_challenge, server_challenge, 8);
	memcpy(ntlmssp->client_challenge, client_challenge, 8);

	ntlmssp->target_info.data = target_info_data;
	ntlmssp->target_info.length = sizeof(target_info_data);

	ntlmssp_compute_lm_v2_response(ntlmssp);
	ntlmssp_compute_ntlm_v2_response(ntlmssp);

	session_base_key_good = 1;
	p = (char*) ntlmssp->session_base_key;
	for (i = 0; i < 16; i++) {
		if (p[i] != expected_session_base_key[i])
			session_base_key_good = 0;
	}

	CU_ASSERT(session_base_key_good == 1);

	lm_challenge_response_good = 1;
	p = (char*) ntlmssp->lm_challenge_response.data;
	for (i = 0; i < 24; i++) {
		if (p[i] != expected_lm_challenge_response[i])
			lm_challenge_response_good = 0;
	}

	CU_ASSERT(lm_challenge_response_good == 1);

	nt_challenge_response_good = 1;
	p = (char*) ntlmssp->nt_challenge_response.data;
	for (i = 0; i < 84; i++) {
		if (p[i] != expected_nt_challenge_response[i])
			nt_challenge_response_good = 0;
	}

	CU_ASSERT(nt_challenge_response_good == 1);
}

void test_ntlmssp_generate_client_signing_key(void)
{
	int i;
	NTLMSSP *ntlmssp;
	int client_signing_key_good;
	uint8 exported_session_key[16] = "\x89\x90\x0d\x5d\x2c\x53\x2b\x36\x31\xcc\x1a\x46\xce\xa9\x34\xf1";
	uint8 expected_client_signing_key[16] = "\xbf\x5e\x42\x76\x55\x68\x38\x97\x45\xd3\xb4\x9f\x5e\x2f\xbc\x89";

	ntlmssp = ntlmssp_new();
	memcpy(ntlmssp->exported_session_key, exported_session_key, sizeof(exported_session_key));

	ntlmssp_generate_client_signing_key(ntlmssp);

	client_signing_key_good = 1;
	for (i = 0; i < 16; i++) {
		if (ntlmssp->client_signing_key[i] != expected_client_signing_key[i])
			client_signing_key_good = 0;
	}

	CU_ASSERT(client_signing_key_good == 1);
}

void test_ntlmssp_generate_server_signing_key(void)
{
	int i;
	NTLMSSP *ntlmssp;
	int server_signing_key_good;
	uint8 exported_session_key[16] = "\x89\x90\x0d\x5d\x2c\x53\x2b\x36\x31\xcc\x1a\x46\xce\xa9\x34\xf1";
	uint8 expected_server_signing_key[16] = "\x9b\x3b\x64\x89\xda\x84\x52\x17\xd5\xc2\x6e\x90\x16\x3b\x42\x11";

	ntlmssp = ntlmssp_new();
	memcpy(ntlmssp->exported_session_key, exported_session_key, sizeof(exported_session_key));

	ntlmssp_generate_server_signing_key(ntlmssp);

	server_signing_key_good = 1;
	for (i = 0; i < 16; i++) {
		if (ntlmssp->server_signing_key[i] != expected_server_signing_key[i])
			server_signing_key_good = 0;
	}

	CU_ASSERT(server_signing_key_good == 1);
}

void test_ntlmssp_generate_client_sealing_key(void)
{
	int i;
	NTLMSSP *ntlmssp;
	int client_sealing_key_good;
	uint8 exported_session_key[16] = "\x89\x90\x0d\x5d\x2c\x53\x2b\x36\x31\xcc\x1a\x46\xce\xa9\x34\xf1";
	uint8 expected_client_sealing_key[16] = "\xca\x41\xcd\x08\x48\x07\x22\x6e\x0d\x84\xc3\x88\xa5\x07\xa9\x73";

	ntlmssp = ntlmssp_new();
	memcpy(ntlmssp->exported_session_key, exported_session_key, sizeof(exported_session_key));

	ntlmssp_generate_client_sealing_key(ntlmssp);

	client_sealing_key_good = 1;
	for (i = 0; i < 16; i++) {
		if (ntlmssp->client_sealing_key[i] != expected_client_sealing_key[i])
			client_sealing_key_good = 0;
	}

	CU_ASSERT(client_sealing_key_good == 1);
}

void test_ntlmssp_generate_server_sealing_key(void)
{
	int i;
	NTLMSSP *ntlmssp;
	int server_sealing_key_good;
	uint8 exported_session_key[16] = "\x89\x90\x0d\x5d\x2c\x53\x2b\x36\x31\xcc\x1a\x46\xce\xa9\x34\xf1";
	uint8 expected_server_sealing_key[16] = "\x14\xb7\x1d\x06\x2c\x68\x2e\xad\x4b\x0e\x95\x23\x70\x91\x98\x90";

	ntlmssp = ntlmssp_new();
	memcpy(ntlmssp->exported_session_key, exported_session_key, sizeof(exported_session_key));

	ntlmssp_generate_server_sealing_key(ntlmssp);

	server_sealing_key_good = 1;
	for (i = 0; i < 16; i++) {
		if (ntlmssp->server_sealing_key[i] != expected_server_sealing_key[i])
			server_sealing_key_good = 0;
	}

	CU_ASSERT(server_sealing_key_good == 1);
}

void test_ntlmssp_encrypt_random_session_key(void)
{
	int i;
	NTLMSSP *ntlmssp;
	int encrypted_random_session_key_good;
	uint8 key_exchange_key[16] = "\x6e\xf1\x6b\x79\x88\xf2\x3d\x7e\x54\x2a\x1a\x38\x4e\xa0\x6b\x52";
	uint8 exported_session_key[16] = "\x89\x90\x0d\x5d\x2c\x53\x2b\x36\x31\xcc\x1a\x46\xce\xa9\x34\xf1";
	uint8 expected_encrypted_random_session_key[16] = "\xb1\xd2\x45\x42\x0f\x37\x9a\x0e\xe0\xce\x77\x40\x10\x8a\xda\xba";

	ntlmssp = ntlmssp_new();
	memcpy(ntlmssp->key_exchange_key, key_exchange_key, 16);
	memcpy(ntlmssp->exported_session_key, exported_session_key, 16);
	memcpy(ntlmssp->random_session_key, exported_session_key, 16);

	ntlmssp_encrypt_random_session_key(ntlmssp);

	encrypted_random_session_key_good = 1;
	for (i = 0; i < 16; i++) {
		if (ntlmssp->encrypted_random_session_key[i] != expected_encrypted_random_session_key[i])
			encrypted_random_session_key_good = 0;
	}

	CU_ASSERT(encrypted_random_session_key_good == 1);
}

void test_ntlmssp_encrypt_message(void)
{
	int i;
	uint8* p;
	NTLMSSP *ntlmssp;
	DATA_BLOB public_key;
	DATA_BLOB ts_credentials;
	DATA_BLOB encrypted_public_key;
	DATA_BLOB encrypted_ts_credentials;
	uint8 public_key_signature[16];
	uint8 ts_credentials_signature[16];
	int encrypted_public_key_good;
	int public_key_signature_good;
	int encrypted_ts_credentials_good;
	int ts_credentials_signature_good;
	uint8 client_signing_key[16] = "\xbf\x5e\x42\x76\x55\x68\x38\x97\x45\xd3\xb4\x9f\x5e\x2f\xbc\x89";
	uint8 client_sealing_key[16] = "\xca\x41\xcd\x08\x48\x07\x22\x6e\x0d\x84\xc3\x88\xa5\x07\xa9\x73";

	uint8 public_key_data[270] =
		"\x30\x82\x01\x0a\x02\x82\x01\x01\x00\xc2\x1c\x54\xaf\x07\xf1\x16"
		"\x97\xc3\x0f\x6b\xa6\x33\x2e\xdd\x1e\xe4\xb2\x9c\xe4\x12\x7f\xda"
		"\x58\x21\xc0\x68\xe6\xd3\xf5\x20\x1c\xba\x06\x64\x7d\x7f\x44\xb5"
		"\xbf\xe3\xd5\xc7\xa4\x86\x8b\xbc\x6f\xca\x25\x78\xdf\xeb\xcf\x5a"
		"\x96\xf6\xc7\x00\xbe\x7d\x6d\x06\x1f\x1d\x7f\x30\xaf\xc4\x59\x4f"
		"\x91\x6d\x97\xe8\x55\x8b\x39\x01\x68\x50\x59\xbb\xe4\x65\x71\x32"
		"\x76\x9e\x1b\xcf\x58\xfc\x52\xd9\x43\x01\x8e\x33\xc1\x74\x14\xbc"
		"\x1f\x5c\x1d\xdb\x0e\xbd\xbb\x37\x50\x13\x78\x57\x93\x34\x3b\x73"
		"\xc9\x5c\x44\x1f\x16\xe6\x2e\x00\x57\xa3\xe6\x5c\x6a\x2c\x90\xdc"
		"\xa3\x6d\x7f\x92\xdf\x2f\xe5\x97\xae\x3b\x07\x23\x03\x91\x71\xd4"
		"\xf2\x50\x3a\x3a\xb9\xde\x1f\xb1\xd5\xa1\x38\x7c\xf7\x07\x49\x83"
		"\x68\xaa\xdf\xad\xfd\x1a\xe9\xb5\x0a\x1e\x8b\xf3\x88\xae\x3f\x32"
		"\xd0\x3b\xd8\xc7\x50\x11\xf7\xad\x3b\x11\xe6\x92\xbb\x2a\x73\x8b"
		"\xed\xfd\x45\x29\x50\xbf\x0d\x1e\x47\xfd\x61\x1d\x18\x27\x58\xa2"
		"\xb2\x1f\xb5\x2d\x84\x18\x2f\x88\x8e\x7f\x70\xed\x4e\xbf\x14\x5d"
		"\x1b\xbc\x0b\x47\x66\x16\x3a\x7b\x6d\x8e\xcf\x55\xe8\x8c\x8a\xfe"
		"\x24\xce\x19\x99\xc3\x5a\xe5\xc2\xf3\x02\x03\x01\x00\x01";

	uint8 expected_encrypted_public_key[270] =
		"\x27\x29\x73\xa9\xfa\x46\x17\x3c\x74\x14\x45\x2a\xd1\xe2\x92\xa1"
		"\xc6\x0a\x30\xd4\xcc\xe0\x92\xf6\xb3\x20\xb3\xa0\xf1\x38\xb1\xf4"
		"\xe5\x96\xdf\xa1\x65\x5b\xd6\x0c\x2a\x86\x99\xcc\x72\x80\xbd\xe9"
		"\x19\x1f\x42\x53\xf6\x84\xa3\xda\x0e\xec\x10\x29\x15\x52\x5c\x77"
		"\x40\xc8\x3d\x44\x01\x34\xb6\x0a\x75\x33\xc0\x25\x71\xd3\x25\x38"
		"\x3b\xfc\x3b\xa8\xcf\xba\x2b\xf6\x99\x0e\x5f\x4e\xa9\x16\x2b\x52"
		"\x9f\xbb\x76\xf8\x03\xfc\x11\x5e\x36\x83\xd8\x4c\x9a\xdc\x9d\x35"
		"\xe2\xc8\x63\xa9\x3d\x07\x97\x52\x64\x54\x72\x9e\x9a\x8c\x56\x79"
		"\x4a\x78\x91\x0a\x4c\x52\x84\x5a\x4a\xb8\x28\x0b\x2f\xe6\x89\x7d"
		"\x07\x3b\x7b\x6e\x22\xcc\x4c\xff\xf4\x10\x96\xf2\x27\x29\xa0\x76"
		"\x0d\x4c\x7e\x7a\x42\xe4\x1e\x6a\x95\x7d\x4c\xaf\xdb\x86\x49\x5c"
		"\xbf\xc2\x65\xb6\xf2\xed\xae\x8d\x57\xed\xf0\xd4\xcb\x7a\xbb\x23"
		"\xde\xe3\x43\xea\xb1\x02\xe3\xb4\x96\xe9\xe7\x48\x69\xb0\xaa\xec"
		"\x89\x38\x8b\xc2\xbd\xdd\xf7\xdf\xa1\x37\xe7\x34\x72\x7f\x91\x10"
		"\x14\x73\xfe\x32\xdc\xfe\x68\x2b\xc0\x08\xdf\x05\xf7\xbd\x46\x33"
		"\xfb\xc9\xfc\x89\xaa\x5d\x25\x49\xc8\x6e\x86\xee\xc2\xce\xc4\x8e"
		"\x85\x9f\xe8\x30\xb3\x86\x11\xd5\xb8\x34\x4a\xe0\x03\xe5";

	uint8 expected_public_key_signature[16] =
		"\x01\x00\x00\x00\x91\x5e\xb0\x6e\x72\x82\x53\xae\x00\x00\x00\x00";

	uint8 ts_credentials_data[65] =
		"\x30\x3f\xa0\x03\x02\x01\x01\xa1\x38\x04\x36\x30\x34\xa0\x0a\x04"
		"\x08\x77\x00\x69\x00\x6e\x00\x37\x00\xa1\x12\x04\x10\x75\x00\x73"
		"\x00\x65\x00\x72\x00\x6e\x00\x61\x00\x6d\x00\x65\x00\xa2\x12\x04"
		"\x10\x70\x00\x61\x00\x73\x00\x73\x00\x77\x00\x6f\x00\x72\x00\x64"
		"\x00";

	uint8 expected_encrypted_ts_credentials[65] =
		"\xa8\x85\x7d\x11\xef\x92\xa0\xd6\xff\xee\xa1\xae\x6d\xc5\x2e\x4e"
		"\x65\x50\x28\x93\x75\x30\xe1\xc3\x37\xeb\xac\x1f\xdd\xf3\xe0\x92"
		"\xf6\x21\xbc\x8f\xa8\xd4\xe0\x5a\xa6\xff\xda\x09\x50\x24\x0d\x8f"
		"\x8f\xf4\x92\xfe\x49\x2a\x13\x52\xa6\x52\x75\x50\x8d\x3e\xe9\x6b"
		"\x57";

	uint8 expected_ts_credentials_signature[16] =
		"\x01\x00\x00\x00\xb3\x2c\x3b\xa1\x36\xf6\x55\x71\x01\x00\x00\x00";

	public_key.data = public_key_data;
	public_key.length = sizeof(public_key_data);

	ntlmssp = ntlmssp_new();
	memcpy(ntlmssp->client_signing_key, client_signing_key, 16);
	memcpy(ntlmssp->client_sealing_key, client_sealing_key, 16);
	ntlmssp_init_rc4_seal_states(ntlmssp);

	ntlmssp_encrypt_message(ntlmssp, &public_key, &encrypted_public_key, public_key_signature);

	p = (uint8*) encrypted_public_key.data;
	encrypted_public_key_good = 1;
	for (i = 0; i < encrypted_public_key.length; i++) {
		if (p[i] != expected_encrypted_public_key[i])
			encrypted_public_key_good = 0;
	}

	CU_ASSERT(encrypted_public_key_good == 1);

	public_key_signature_good = 1;
	for (i = 0; i < 16; i++) {
		if (public_key_signature[i] != expected_public_key_signature[i])
			public_key_signature_good = 0;
	}

	CU_ASSERT(public_key_signature_good == 1);

	ts_credentials.data = ts_credentials_data;
	ts_credentials.length = sizeof(ts_credentials_data);

	ntlmssp_encrypt_message(ntlmssp, &ts_credentials, &encrypted_ts_credentials, ts_credentials_signature);

	p = (uint8*) encrypted_ts_credentials.data;
	encrypted_ts_credentials_good = 1;
	for (i = 0; i < encrypted_ts_credentials.length; i++) {
		if (p[i] != expected_encrypted_ts_credentials[i])
			encrypted_ts_credentials_good = 0;
	}

	CU_ASSERT(encrypted_ts_credentials_good == 1);

	ts_credentials_signature_good = 1;
	for (i = 0; i < 16; i++) {
		if (ts_credentials_signature[i] != expected_ts_credentials_signature[i])
			ts_credentials_signature_good = 0;
	}

	CU_ASSERT(ts_credentials_signature_good == 1);
}

void test_ntlmssp_decrypt_message(void)
{
	int i;
	uint8* p;
	NTLMSSP *ntlmssp;
	DATA_BLOB public_key;
	DATA_BLOB encrypted_public_key;
	int public_key_good;
	uint8 server_signing_key[16] = "\x9b\x3b\x64\x89\xda\x84\x52\x17\xd5\xc2\x6e\x90\x16\x3b\x42\x11";
	uint8 server_sealing_key[16] = "\x14\xb7\x1d\x06\x2c\x68\x2e\xad\x4b\x0e\x95\x23\x70\x91\x98\x90";

	uint8 encrypted_public_key_data[270] =
		"\xc7\x51\xf4\x71\xd3\x9f\xb6\x50\xbe\xa8\xf6\x20\x77\xa1\xfc\xdd"
		"\x8e\x02\xf0\xa4\x6b\xba\x3f\x9d\x65\x9d\xab\x4a\x95\xc9\xb4\x38"
		"\x03\x87\x04\xb1\xfe\x42\xec\xfa\xfc\xaa\x85\xf1\x31\x2d\x26\xcf"
		"\x63\xfd\x62\x36\xcf\x56\xc3\xfb\xf6\x36\x9b\xe5\xb2\xe7\xce\xcb"
		"\xe1\x82\xb2\x89\xff\xdd\x87\x5e\xd3\xd8\xff\x2e\x16\x35\xad\xdb"
		"\xda\xc9\xc5\x81\xad\x48\xf1\x8b\x76\x3d\x74\x34\xdf\x80\x6b\xf3"
		"\x68\x6d\xf6\xec\x5f\xbe\xea\xb7\x6c\xea\xe4\xeb\xe9\x17\xf9\x4e"
		"\x0d\x79\xd5\x82\xdd\xb7\xdc\xcd\xfc\xbb\xf1\x0b\x9b\xe9\x18\xe7"
		"\xb3\xb3\x8b\x40\x82\xa0\x9d\x58\x73\xda\x54\xa2\x2b\xd2\xb6\x41"
		"\x60\x8a\x64\xf2\xa2\x59\x64\xcf\x27\x1a\xe6\xb5\x1a\x0e\x0e\xe1"
		"\x14\xef\x26\x68\xeb\xc8\x49\xe2\x66\xbb\x11\x71\x49\xad\x7e\xae"
		"\xde\xa8\x78\xfd\x64\x51\xd8\x18\x01\x11\xc0\x8d\x3b\xec\x40\x2b"
		"\x1f\xc5\xa4\x45\x1e\x07\xae\x5a\xd8\x1c\xab\xdf\x89\x96\xdc\xdc"
		"\x29\xd8\x30\xdb\xbf\x48\x2a\x42\x27\xc2\x50\xac\xf9\x02\xd1\x20"
		"\x12\xdd\x50\x22\x09\x44\xac\xe0\x22\x1f\x66\x64\xec\xfa\x2b\xb8"
		"\xcd\x43\x3a\xce\x40\x74\xe1\x34\x81\xe3\x94\x47\x6f\x49\x01\xf8"
		"\xb5\xfc\xd0\x75\x80\xc6\x35\xac\xc0\xfd\x1b\xb5\xa2\xd3";

	uint8 expected_public_key[270] =
		"\x31\x82\x01\x0a\x02\x82\x01\x01\x00\xc2\x1c\x54\xaf\x07\xf1\x16"
		"\x97\xc3\x0f\x6b\xa6\x33\x2e\xdd\x1e\xe4\xb2\x9c\xe4\x12\x7f\xda"
		"\x58\x21\xc0\x68\xe6\xd3\xf5\x20\x1c\xba\x06\x64\x7d\x7f\x44\xb5"
		"\xbf\xe3\xd5\xc7\xa4\x86\x8b\xbc\x6f\xca\x25\x78\xdf\xeb\xcf\x5a"
		"\x96\xf6\xc7\x00\xbe\x7d\x6d\x06\x1f\x1d\x7f\x30\xaf\xc4\x59\x4f"
		"\x91\x6d\x97\xe8\x55\x8b\x39\x01\x68\x50\x59\xbb\xe4\x65\x71\x32"
		"\x76\x9e\x1b\xcf\x58\xfc\x52\xd9\x43\x01\x8e\x33\xc1\x74\x14\xbc"
		"\x1f\x5c\x1d\xdb\x0e\xbd\xbb\x37\x50\x13\x78\x57\x93\x34\x3b\x73"
		"\xc9\x5c\x44\x1f\x16\xe6\x2e\x00\x57\xa3\xe6\x5c\x6a\x2c\x90\xdc"
		"\xa3\x6d\x7f\x92\xdf\x2f\xe5\x97\xae\x3b\x07\x23\x03\x91\x71\xd4"
		"\xf2\x50\x3a\x3a\xb9\xde\x1f\xb1\xd5\xa1\x38\x7c\xf7\x07\x49\x83"
		"\x68\xaa\xdf\xad\xfd\x1a\xe9\xb5\x0a\x1e\x8b\xf3\x88\xae\x3f\x32"
		"\xd0\x3b\xd8\xc7\x50\x11\xf7\xad\x3b\x11\xe6\x92\xbb\x2a\x73\x8b"
		"\xed\xfd\x45\x29\x50\xbf\x0d\x1e\x47\xfd\x61\x1d\x18\x27\x58\xa2"
		"\xb2\x1f\xb5\x2d\x84\x18\x2f\x88\x8e\x7f\x70\xed\x4e\xbf\x14\x5d"
		"\x1b\xbc\x0b\x47\x66\x16\x3a\x7b\x6d\x8e\xcf\x55\xe8\x8c\x8a\xfe"
		"\x24\xce\x19\x99\xc3\x5a\xe5\xc2\xf3\x02\x03\x01\x00\x01";

	uint8 public_key_signature[16] =
		"\x01\x00\x00\x00\xc9\x88\xfc\xf1\x11\x68\x2c\x72\x00\x00\x00\x00";

	encrypted_public_key.data = encrypted_public_key_data;
	encrypted_public_key.length = sizeof(encrypted_public_key_data);

	ntlmssp = ntlmssp_new();
	memcpy(ntlmssp->server_signing_key, server_signing_key, 16);
	memcpy(ntlmssp->server_sealing_key, server_sealing_key, 16);
	ntlmssp_init_rc4_seal_states(ntlmssp);

	ntlmssp_decrypt_message(ntlmssp, &encrypted_public_key, &public_key, public_key_signature);

	p = (uint8*) public_key.data;
	public_key_good = 1;
	for (i = 0; i < public_key.length; i++) {
		if (p[i] != expected_public_key[i])
			public_key_good = 0;
	}

	CU_ASSERT(public_key_good == 1);
}
