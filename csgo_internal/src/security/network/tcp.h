#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <wolfssl/IDE/WIN/user_settings.h>
#include <wolfssl/ssl.h>
#include "packet.h"
#include <iostream>
#include "events.h"
#include "../../util/console/logger.h"

namespace tcp {
	enum client_state {
		connecting = 0, idle, logging_in, logged_in, imports_ready, waiting, image_ready, injected, blacklisted
	};

	enum login_result {
		login_fail = 15494,
		hwid_mismatch = 11006,
		login_success = 61539,
		group_id_mismatch = 34385,
		banned = 28618,
		server_error = 98679
	};

	struct breakwhile {
	public:
		bool m_break{ false };
	};
	inline breakwhile termination;

	class client {
		int m_socket;
		std::atomic<bool> m_active;

		WOLFSSL* m_server_ssl;
		WOLFSSL_CTX* m_ssl_ctx;
	public:
		int state;
		std::string username;
		int login_result;

		std::string session_id;
		event<packet_t> receive_event;
		event<> connect_event;

		std::string ver = "650117120";

		client() : m_socket{ -1 }, m_active{ false }, state{ client_state::connecting }, m_server_ssl{ nullptr }, m_ssl_ctx{ nullptr }, login_result{ -1 } {}

		void start(const std::string_view server_ip, const uint16_t port);

		__forceinline int write(const packet_t& packet) {
			if (!packet) return 0;
			return write(packet.message.data(), packet.message.size());
		}

		__forceinline int write(const void* data, int size) {
			return wolfSSL_write(m_server_ssl, data, size);
		}

		__forceinline int read(void* data, int size) {
			return wolfSSL_read(m_server_ssl, data, size);
		}

		int read_stream(std::vector<char>& out);
		int stream(std::vector<char>& data);

		__forceinline int stream(const std::string_view str) {
			std::vector<char> vec(str.begin(), str.end());
			return stream(vec);
		}

		__forceinline int read_stream(std::string& str) {
			std::vector<char> out;
			int ret = read_stream(out);
			str.assign(out.begin(), out.end());
			return ret;
		}

		__forceinline int get_socket() { return m_socket; }

		operator bool() { return m_active.load(); }

		__forceinline void shutdown() {
			m_active.store(false);

			if (m_server_ssl) {
				closesocket(m_socket);
				wolfSSL_shutdown(m_server_ssl);
				wolfSSL_free(m_server_ssl);

				m_socket = -1;
				m_server_ssl = nullptr;
			}

			termination.m_break = true;
		}
	};
	inline client _client;

}  // namespace tcp

