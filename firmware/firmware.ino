#include "Main.h"

namespace Purser
{
	namespace Buffer
	{
		// NOTE:
		// ---
		// バッファサイズはBLEのペイロード(20byte)以上ならばOK
		inline static const int LENGTH()       { return 32; }
		#define        _BUFFER__LENGTH                  32
		inline static const int TO_NUM__BASE() { return 16; } // 数値に変換する際の基数
	}
}

/**
 * コンフィグメソッド・変数
 * =============================================================================
 */
namespace Config
{
	bool          enable   = false;

	unsigned char joint_id = 0;
	unsigned int  angle    = 0;

	Motion::HeaderDefinition       header = { 0 };
	Motion::Frame::FrameDefinition frame  = { 0 };

	bool disable()
	{
		return !enable;
	}
}

/**
 * モーション管理メソッド・変数
 * =============================================================================
 * NOTE:
 * ---
 */
namespace Motion
{
	bool _playing = false;
	HeaderDefinition header = { 0 };

	namespace Frame
	{
		unsigned int  transition_count = 0;
		volatile bool _updatable       = false;

		FrameDefinition  buffer[3] = { 0 };
		FrameDefinition* now  = buffer;
		FrameDefinition* next = buffer + 1;
		FrameDefinition* back = buffer + 2;

		long now_fixed_point[_JOINT__SUM]  = { 0 };
		long diff_fixed_point[_JOINT__SUM] = { 0 };

		bool updatable()
		{
			return _updatable;
		}

		bool updateFinished()
		{
			return (transition_count == 0);
		}

		bool nextFrameLoadable()
		{
			if (header.extra[0] != 0) return true;

			return ((Frame::next->number + 1) < header.frame_num);
		}

		void buffering()
		{
			FrameDefinition* temp = now;
			now  = next;
			next = back;
			back = temp;
		}

		void update()
		{
			_updatable = false;
			transition_count--;

			for (int index = 0; index < Joint::SUM(); index++)
			{
				now_fixed_point[index] += diff_fixed_point[index];
				now->joint_angle[index] = now_fixed_point[index] >> 16;
			}
		}
	}


	void play(unsigned char motion_slot)
	{
		_playing = true;

		getHeader(motion_slot, &header);
		getFrame(motion_slot, 0, Frame::next);

		if (header.frame_num > FLAMENUM_MIN())
		{
			getFrame(motion_slot, 1, Frame::back);

			for (int index = 0; index < Joint::SUM(); index++)
			{
				Frame::back->joint_angle[index] += Joint::SETTINGS[index].HOME;
			}
		}

		Frame::transition_count = Frame::next->transition_delay_msec / Frame::UPDATE_MSEC();

		for (int index = 0; index < Joint::SUM(); index++)
		{
			Frame::next->joint_angle[index] += Joint::SETTINGS[index].HOME;

			Frame::now_fixed_point[index]  = (long)(Frame::now->joint_angle[index]) << 16;
			Frame::diff_fixed_point[index] = (((long)(Frame::next->joint_angle[index]) << 16) - Frame::now_fixed_point[index]);
			Frame::diff_fixed_point[index] /= Frame::transition_count;
		}
	}

	void stop()
	{
		_playing = false;
	}

	bool playing()
	{
		return _playing;
	}

	// モーション定義情報を外部EEPROMへ書き込みます。
	// TASK: 定数定義の分離
	// CAUTION: ヘッダーサイズの変化に弱い実装
	void setHeader(HeaderDefinition* p_header)
	{
		#if _DEBUG_EXTEEPROM
			System::output_serial->println(F("in fuction : Motion::setHeader()"));
		#endif

		char* filler = (char*)p_header;

		int ret = ExternalEEPROM::writeBlock((int)p_header->slot * 41, filler, sizeof(*p_header) /* = 25byte */);
		delay(10);

		#if _DEBUG_EXTEEPROM
			System::output_serial->print(F("return : "));
			System::output_serial->println(ret);
		#endif
	}

	// モーション定義情報を外部EEPROMから読み込みます。
	// TASK: 定数定義の分離
	// CAUTION: ヘッダーサイズの変化に弱い実装
	void getHeader(unsigned char motion_slot, HeaderDefinition* p_header)
	{
		#if _DEBUG_EXTEEPROM
			System::output_serial->println(F("in fuction : Motion::getHeader()"));
		#endif

		char* filler = (char*)p_header;

		int ret = ExternalEEPROM::readBlock((int)motion_slot * 41, filler, sizeof(*p_header) /* = 25byte */);

		#if _DEBUG_EXTEEPROM
			System::output_serial->print(F("return : "));
			System::output_serial->println(ret);
		#endif

		#ifdef _DEBUG_MOTION
			System::output_serial->print("slot(arg) : ");
			System::output_serial->println(motion_slot);
			System::output_serial->print("slot(get) : ");
			System::output_serial->println(p_header->slot);
		#endif
	}

	// フレームを外部EEPROMへ書き込みます。
	// TASK: 定数定義の分離
	// CAUTION: フレームサイズの変化に弱い実装
	void setFrame(unsigned char motion_slot, Frame::FrameDefinition* p_frame)
	{
		#if _DEBUG_EXTEEPROM
			System::output_serial->println(F("in fuction : Motion::setFrame()"));
		#endif

		char* filler = (char*)p_frame;

		int ret = ExternalEEPROM::writeBlock((int)motion_slot * 41 + 1 + (int)p_frame->number * 2, filler, 30);
		#if _DEBUG_EXTEEPROM
			System::output_serial->print(F("return [0] : "));
			System::output_serial->println(ret);
		#endif
		delay(10);

		ret = ExternalEEPROM::writeBlock((int)motion_slot * 41 + 1 + (int)p_frame->number * 2 + 1, filler + 30, 21);
		#if _DEBUG_EXTEEPROM
			System::output_serial->print(F("return [1] : "));
			System::output_serial->println(ret);
		#endif
		delay(10);
	}

	// フレームを外部EEPROMから読み込みます。
	// TASK: 定数定義の分離
	// CAUTION: フレームサイズの変化に弱い実装
	void getFrame(unsigned char motion_slot, unsigned char frame_num, Frame::FrameDefinition* p_frame)
	{
		#if _DEBUG_EXTEEPROM
			System::output_serial->println(F("in fuction : Motion::getFrame()"));
		#endif

		char* filler = (char*)p_frame;

		int ret = ExternalEEPROM::readBlock((int)motion_slot * 41 + 1 + (int)frame_num * 2, filler, 30);
		#if _DEBUG_EXTEEPROM
			System::output_serial->print(F("return [0] : "));
			System::output_serial->println(ret);
		#endif

		ret = ExternalEEPROM::readBlock((int)motion_slot * 41 + 1 + (int)frame_num * 2 + 1, filler + 30, 21);
		#if _DEBUG_EXTEEPROM
			System::output_serial->print(F("return [1] : "));
			System::output_serial->println(ret);
		#endif
	}
}


/**
 * コードインタプリタの定義
 * =============================================================================
 */
namespace Code
{
	inline static const int CODE_SIZE() { return 2;  }
	inline static const int STACK_MAX() { return 64; }

	class CodeModel {
	public:
		unsigned char slot;
		unsigned char loop;
	};

	CodeModel codes[64] = { 0 };

	char stack = 0;
	char stack_index = 0;
	bool _running = false;

	unsigned char _loop;

	void init()
	{
		#ifdef _DEBUG_CODE
			System::output_serial->println(F("in fuction : Code::init()"));
		#endif

		stack = 0;
		stack_index = 0;
		_running = false;
	}

	bool running()
	{
		return _running;
	}

	bool setCode(unsigned char slot, unsigned char loop)
	{
		#ifdef _DEBUG_CODE
			System::output_serial->println(F("in fuction : Code::setCode()"));
		#endif

		if (stack == STACK_MAX())
		{
			return false;
		}

		stack++;

		#ifdef _DEBUG_CODE
			System::output_serial->print(F(">>> stack : "));
			System::output_serial->println((int)stack);

			System::output_serial->print(F(">>> slot : "));
			System::output_serial->println((int)slot);

			System::output_serial->print(F(">>> loop : "));
			System::output_serial->println((int)loop);
		#endif

		codes[stack - 1].slot = slot;
		codes[stack - 1].loop = loop;

		return true;
	}

	bool getCode(unsigned char& slot, unsigned char& loop)
	{
		#ifdef _DEBUG_CODE
			System::output_serial->println(F("in fuction : Code::getCode()"));
		#endif

		if (stack == 0)
		{
			return false;
		}

		slot = codes[stack_index].slot;
		loop = codes[stack_index].loop;
		stack_index++;

		#ifdef _DEBUG_CODE
			System::output_serial->print(F(">>> stack : "));
			System::output_serial->println((int)stack);

			System::output_serial->print(F(">>> slot : "));
			System::output_serial->println((int)slot);

			System::output_serial->print(F(">>> loop : "));
			System::output_serial->println((int)loop);
		#endif

		stack--;

		return true;
	}

	void run()
	{
		#ifdef _DEBUG_CODE
			System::output_serial->println(F("in fuction : Code::run()"));
		#endif

		if (!Motion::playing())
		{
			stack_index = 0;
			unsigned char slot;

			if (getCode(slot, _loop))
			{
				#ifdef _DEBUG_CODE
					System::output_serial->println(F(">>> getCode() == true"));
				#endif

				Motion::play(slot);
				if (Motion::header.extra[0] != 1)
				{
					Motion::header.extra[0] = 1;
					Motion::header.extra[1] = 0;
					Motion::header.extra[2] = Motion::header.frame_num - 1;
				}

				_running = true;
			}
		}
	}
}

namespace Utility {
	bool stop = false;

	void motionStopHelper()
	{
		stop = true;
	}

	void motionPlayHelper()
	{
		stop = false;
	}
}

/**
 * コマンドパーサの定義
 * =============================================================================
 * NOTE:
 * ---
 * 本ファームウェアにおけるパーサは、Buffer, Token, State, Packet, および
 * それらを操作するためのメソッド群から構成されます。
 *
 * 一般的なオートマトンにおけるトークンの実態が2種類存在し、1つはToken::STR, 
 * もう1つはPacket::DEFINITIONSです。これらの違いは、文字列が実態か，
 * それとも任意のバイト列が実態か、ということです。
 *
 * 状態遷移表はState::TRANSITIONにディスプレイとして定義されています。
 * ここで、メモリサイズを節約するため、バイト列が実態のトークン識別子を0とし、
 * 各遷移テーブルのサイズを最小限に抑えています。
 *
 * メインループでの処理を簡便にするため、字句解析機にあたるlexAccept()メソッドが
 * 内部状態に依存する挙動を行っています。
 *
 * ユーザが編集する必要があるのは、基本的にexecEventHandler()メソッドだけです。
 */
namespace Purser
{
	namespace Buffer
	{
		char data[_BUFFER__LENGTH] = { 0 };
		char position = 0;
	}

	namespace Token
	{
		typedef enum
		{
			OTHER,
			MOTION_PLAY_COMMAND,         // [00] モーションの再生
			SET_CODE_COMMAND,            // [12] コードの設定コマンド
			CODE_RUN_COMMAND,            // [01] コードの実行
			STOP_COMMAND,                // [02] 停止
			MOTION_INSTALL_COMMAND,      // [03] モーションデータのインストール
			JOINT_MIN_SETTING_COMMAND,   // [04] 関節可動域最小値の設定
			JOINT_MAX_SETTING_COMMAND,   // [05] 関節可動域最大値の設定
			JOINT_HOME_SETTING_COMMAND,  // [06] 関節初期位置の設定
			JOINT_MOVE_COMMAND,          // [07] 関節可動命令
			SERIAL_TOGGLE_COMMAND,       // [08] 読み込みシリアルの切り替え
			DUMP_JOINT_SETTING_COMMAND,  // [09] 関節設定のダンプコマンド
			DUMP_MOTION_COMMAND,         // [10] モーションのダンプコマンド
			RESET_JOINT_SETTING_COMMAND, // [11] 関節設定のリセットコマンド
			PURSER_TOKEN_EOF,
			PURSER_ERROR
		} PurserToken;

		const char* STR[] =
		{
			"OTHER",
			"$MP", // [00] モーションの再生
			"#SC", // [12] コードの設定コマンド
			"$CR", // [01] コードの実行
			"$MS", // [02] 停止
			"#IN", // [03] モーションデータのインストール
			"#MI", // [04] 関節可動域最小値の設定
			"#MA", // [05] 関節可動域最大値の設定
			"#HO", // [06] 関節初期位置の設定
			"#SA", // [07] 関節可動命令
			"###", // [08] 読み込みシリアルの切り替え
			"#DJ", // [09] 関節設定のダンプコマンド
			"#DM", // [10] モーションのダンプコマンド
			"#RJ"  // [11] 関節設定のリセットコマンド
		};

		PurserToken initial = OTHER;
		PurserToken now     = OTHER;
	}

	namespace State
	{
		typedef enum
		{
			INIT,
			SLOT_ELEMENT_INCOMING,
			NAME_ELEMENT_INCOMING,
			EXTRA_ELEMENT_INCOMING,
			FRAMENUM_ELEMENT_INCOMING,
			FRAME_ELEMENT_INCOMING,
			JOINT_ID_ELEMENT_INCOMING,
			ANGLE_ELEMENT_INCOMING,
			MOTION_SLOT_ELEMENT_INCOMING,
			CODE_ELEMENT_INCOMING,
			PURSER_STATE_EOF
		} PurserState;

		#if _DEBUG
			const char* STR[] =
			{
				"INIT",
				"SLOT_ELEMENT_INCOMING",
				"NAME_ELEMENT_INCOMING",
				"EXTRA_ELEMENT_INCOMING",
				"FRAMENUM_ELEMENT_INCOMING",
				"FRAME_ELEMENT_INCOMING",
				"JOINT_ID_ELEMENT_INCOMING",
				"ANGLE_ELEMENT_INCOMING",
				"MOTION_SLOT_ELEMENT_INCOMING",
				"CODE_ELEMENT_INCOMING"
			};
		#endif

		const PurserState TRANSITION_INIT[] =
		{
			INIT,                         // トークン"OTHER"入力時の状態遷移先
			MOTION_SLOT_ELEMENT_INCOMING, // トークン"MOTION_PLAY_COMMAND"入力時の状態遷移先
			CODE_ELEMENT_INCOMING,        // トークン"SET_CODE_COMMAND"入力時の状態遷移先
			INIT,                         // トークン"CODE_RUN_COMMAND"入力時の状態遷移先
			INIT,                         // トークン"STOP_COMMAND"入力時の状態遷移先
			SLOT_ELEMENT_INCOMING,        // トークン"MOTION_INSTALL_COMMAND"入力時の状態遷移先
			JOINT_ID_ELEMENT_INCOMING,    // トークン"JOINT_MIN_SETTING_COMMAND"入力時の状態遷移先
			JOINT_ID_ELEMENT_INCOMING,    // トークン"JOINT_MAX_SETTING_COMMAND"入力時の状態遷移先
			JOINT_ID_ELEMENT_INCOMING,    // トークン"JOINT_HOME_SETTING_COMMAND"入力時の状態遷移先
			JOINT_ID_ELEMENT_INCOMING,    // トークン"JOINT_MOVE_COMMAND"入力時の状態遷移先
			INIT,                         // トークン"SERIAL_TOGGLE_COMMAND"入力時の状態遷移先
			INIT,                         // トークン"DUMP_JOINT_SETTING_COMMAND"入力時の状態遷移先
			MOTION_SLOT_ELEMENT_INCOMING, // トークン"DUMP_MOTION_COMMAND"入力時の状態遷移先
			INIT                          // トークン"RESET_JOINT_SETTING_COMMAND"入力時の状態遷移先
		};

		const PurserState TRANSITION_SLOT_ELEMENT_INCOMING[] =
		{
			NAME_ELEMENT_INCOMING          // トークン"OTHER"入力時の状態遷移先
		};

		const PurserState TRANSITION_NAME_ELEMENT_INCOMING[] =
		{
			EXTRA_ELEMENT_INCOMING         // トークン"OTHER"入力時の状態遷移先
		};

		const PurserState TRANSITION_EXTRA_ELEMENT_INCOMING[] =
		{
			FRAMENUM_ELEMENT_INCOMING      // トークン"OTHER"入力時の状態遷移先
		};

		const PurserState TRANSITION_FRAMENUM_ELEMENT_INCOMING[] =
		{
			FRAME_ELEMENT_INCOMING         // トークン"OTHER"入力時の状態遷移先
		};

		const PurserState TRANSITION_FRAME_ELEMENT_INCOMING[] =
		{
			INIT                           // トークン"OTHER"入力時の状態遷移先
		};

		const PurserState TRANSITION_JOINT_ID_ELEMENT_INCOMING[] =
		{
			ANGLE_ELEMENT_INCOMING         // トークン"OTHER"入力時の状態遷移先
		};

		const PurserState TRANSITION_ANGLE_ELEMENT_INCOMING[] =
		{
			INIT                           // トークン"OTHER"入力時の状態遷移先
		};

		const PurserState TRANSITION_MOTION_SLOT_ELEMENT_INCOMING[] =
		{
			INIT                           // トークン"OTHER"入力時の状態遷移先
		};

		const PurserState TRANSITION_CODE_ELEMENT_INCOMING[] = 
		{
			INIT                           // トークン"OTHER"入力時の状態遷移先
		};

		const PurserState* TRANSITION[] =
		{
			TRANSITION_INIT,
			TRANSITION_SLOT_ELEMENT_INCOMING,
			TRANSITION_NAME_ELEMENT_INCOMING,
			TRANSITION_EXTRA_ELEMENT_INCOMING,
			TRANSITION_FRAMENUM_ELEMENT_INCOMING,
			TRANSITION_FRAME_ELEMENT_INCOMING,
			TRANSITION_JOINT_ID_ELEMENT_INCOMING,
			TRANSITION_ANGLE_ELEMENT_INCOMING,
			TRANSITION_MOTION_SLOT_ELEMENT_INCOMING,
			TRANSITION_CODE_ELEMENT_INCOMING
		};

		PurserState now = INIT;
	}

	namespace Packet
	{
		struct PacketDefinition
		{
			const char bytes; // パケットサイズ
			int count;        // パケット受信予定数 (-1を定義した場合、プレースホルダと見なす)
		};

		PacketDefinition DEFINITIONS[] =
		{
			{ 3,  1  }, // 状態"INIT"において到着するパケット
			{ 2,  1  }, // 状態"SLOT_ELEMENT_INCOMING"において到着するパケット
			{ 20, 1  }, // 状態"NAME_ELEMENT_INCOMING"において到着するパケット
			{ 2,  3  }, // 状態"EXTRA_ELEMENT_INCOMING"において到着するパケット
			{ 2,  1  }, // 状態"FRAME_NUM_ELEMENT_INCOMING"において到着するパケット
			{ 4,  -1 }, // 状態"FRAME_ELEMENT_INCOMING"において到着するパケット
			{ 2,  1  }, // 状態"JOINT_ID_ELEMENT_INCOMING"において到着するパケット
			{ 3,  1  }, // 状態"ANGLE_ELEMENT_INCOMING"において到着するパケット
			{ 2,  1  }, // 状態"MOTION_SLOT_ELEMENT_INCOMING"において到着するパケット
			{ 2,  2  }  // 状態"CODE_ELEMENT_INCOMING"において到着するパケット
		};
	}

	// パーサーの初期化を行います。
	// arduinoのsetup()内で呼ぶ必要はありません。
	void init()
	{
		#if _DEBUG
			System::output_serial->println(F("in fuction : Purser::init()"));
		#endif

		memset(Buffer::data, '\0', Buffer::LENGTH()); // Sanity check.
		Buffer::position = 0;
		
		State::now = State::INIT;
		Token::now = Token::OTHER;

		Packet::DEFINITIONS[State::INIT].count                          = 1;
		Packet::DEFINITIONS[State::SLOT_ELEMENT_INCOMING].count         = 1;
		Packet::DEFINITIONS[State::NAME_ELEMENT_INCOMING].count         = 1;
		Packet::DEFINITIONS[State::EXTRA_ELEMENT_INCOMING].count        = 3;
		Packet::DEFINITIONS[State::FRAMENUM_ELEMENT_INCOMING].count     = 1;
		Packet::DEFINITIONS[State::FRAME_ELEMENT_INCOMING].count        = -1;
		Packet::DEFINITIONS[State::JOINT_ID_ELEMENT_INCOMING].count     = 1;
		Packet::DEFINITIONS[State::ANGLE_ELEMENT_INCOMING].count        = 1;
		Packet::DEFINITIONS[State::MOTION_SLOT_ELEMENT_INCOMING].count  = 1;
		Packet::DEFINITIONS[State::CODE_ELEMENT_INCOMING].count         = 2;
	}

	// 1バイトをリングバッファ(もどき)に格納します。
	// 本来リングバッファにする必要はないので、オーバーヘッドが気になる場合は
	// 内部のif文を削除してください。
	void readByte(char byte)
	{
		#if _DEBUG
			System::output_serial->println(F("in fuction : Purser::readByte()"));
		#endif

		if ((State::now == State::INIT) && (Buffer::position == 0))
		{
			if ((byte != '#') && (byte != '$')) return;
		}

		if (Buffer::position == Buffer::LENGTH()) Buffer::position = 0; // Sanity check.

		Buffer::data[Buffer::position++] = byte;
	}

	// 字句解析を行い、入力された文字列を受理するか判定します。
	// ユーザが内部を編集する必要は基本的にありません。
	bool lexAccept()
	{
		#if _DEBUG
			System::output_serial->println(F("in fuction : Purser::lexAccept()"));
		#endif

		bool bytes_accepted = (Buffer::position == Packet::DEFINITIONS[State::now].bytes);

		if (bytes_accepted && (State::now == State::INIT))
		{
			bool token_accepted = false;

			for (
				int token = Token::MOTION_PLAY_COMMAND;
				token != Token::PURSER_TOKEN_EOF;
				token++
			)
			{
				if (strncmp(Buffer::data, Token::STR[token], Packet::DEFINITIONS[State::INIT].bytes) == 0)
				{
					Token::now = (Token::PurserToken)token;
					token_accepted = true;
				}
			}

			Buffer::position = 0;

			return token_accepted;
		}

		Token::now = Token::OTHER;

		return bytes_accepted;
	}

	// トークン履歴を作成します。
	// 先に到着したトークンに依存する処理を実現するために必要です。
	void makeTokenLog()
	{
		#if _DEBUG
			System::output_serial->println(F("in fuction : Purser::makeTokenLog()"));
		#endif

		// コマンド受理時のトークンを履歴に保持
		if (State::now == State::INIT)
		{
			Token::initial = Token::now;
		}
	}

	// パーサーのエラー時処理を委譲します。
	void errorAbort()
	{
		#if _DEBUG
			System::output_serial->println(F("in fuction : Purser::errorAbort()"));
		#endif

		Token::now = Token::PURSER_ERROR;
	}

	// 状態ごとのイベントハンドラを実行します。
	// 各状態に対応して処理をcase文内部に記述してください。
	void execEventHandler()
	{
		#if _DEBUG
			System::output_serial->println(F("in fuction : Purser::execEventHandle()"));
		#endif

		// 到着バイト列の末尾にnull文字を入れることで文字列化
		Buffer::data[Packet::DEFINITIONS[State::now].bytes] = '\0';

		switch (State::now)
		{
			case State::INIT:
			{
				switch (Token::initial)
				{
					// これらのコマンドはコンフィグフレームを描画するために、共通のswitch文に通す。
					case Token::JOINT_MIN_SETTING_COMMAND:
					case Token::JOINT_MAX_SETTING_COMMAND:
					case Token::JOINT_HOME_SETTING_COMMAND:
					case Token::JOINT_MOVE_COMMAND:
					{
						// 非コンフィグモードからコンフィグモードに移ったときのみ、コンフィグフレームを初期化
						if (Config::enable == false)
						{
							Utility::motionPlayHelper();

							for (int index = 0; index < Joint::SUM(); index++)
							{
								Config::frame.joint_angle[index] = Joint::SETTINGS[index].HOME;
							}

							Config::enable = true;
						}

						break;
					}

					case Token::MOTION_INSTALL_COMMAND:
					{
						// 割り込みの優先順序を考慮し、モーションの再生を中断
						Utility::motionStopHelper();

						break;
					}

					case Token::SERIAL_TOGGLE_COMMAND:
					{
						System::toggleInputSerial();

						break;
					}

					case Token::DUMP_JOINT_SETTING_COMMAND:
					{
						Config::dumpJointSettings();

						break;
					}

					case Token::DUMP_MOTION_COMMAND:
					{
						// 割り込みの優先順位を考慮し、モーションの再生を中断
						Utility::motionStopHelper();

						break;
					}

					case Token::RESET_JOINT_SETTING_COMMAND:
					{
						Config::resetJointSettings();

						break;
					}

					case Token::CODE_RUN_COMMAND:
					{
						if (!Code::running())
						{
							Code::run();
						}

						break;
					}

					case Token::STOP_COMMAND:
					{
						if (Motion::playing())
						{
							Motion::header.extra[0] = 0;
						}

						break;
					}

					default:
					{
						Config::enable = false;

						break;
					}
				}

				break;
			}

			case State::SLOT_ELEMENT_INCOMING:
			{
				unsigned char slot = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());

				if (slot > Motion::SLOT_MAX())
				{
					#if _DEBUG_INSTALL
						System::output_serial->println(F(">>> slot : bad argment (value : "));
						System::output_serial->print((int)slot);
						System::output_serial->println(F(")"));
					#endif

					Config::header.slot = Motion::SLOT_MAX();
				}
				else
				{
					Config::header.slot = slot;
				}

				break;
			}

			case State::NAME_ELEMENT_INCOMING:
			{
				memcpy(Config::header.name, Buffer::data, 20); // TASK: 定数定義の分離

				break;
			}

			case State::EXTRA_ELEMENT_INCOMING:
			{
				unsigned char extra = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());
				Config::header.extra[3 - Packet::DEFINITIONS[State::now].count] = extra; // TASK: 定数定義の分離

				break;
			}

			case State::FRAMENUM_ELEMENT_INCOMING:
			{
				unsigned char frame_num = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());

				if (frame_num > Motion::FLAMENUM_MAX())
				{
					#if _DEBUG_INSTALL
						System::output_serial->println(F(">>> frame_num : bad argment (value : "));
						System::output_serial->print((int)frame_num);
						System::output_serial->println(F(")"));
					#endif

					Config::header.frame_num = Motion::FLAMENUM_MAX();
				}
				else if (frame_num < Motion::FLAMENUM_MIN())
				{
					#if _DEBUG_INSTALL
						System::output_serial->println(F(">>> frame_num : bad argment (value : "));
						System::output_serial->print((int)frame_num);
						System::output_serial->println(F(")"));
					#endif

					Config::header.frame_num = Motion::FLAMENUM_MIN();

					for (int index = 0; index < Joint::SUM(); index++)
					{
						Config::frame.joint_angle[index] = Joint::SETTINGS[index].HOME;
					}

					Motion::setHeader(&Config::header);
					Motion::setFrame(Config::header.slot, &Config::frame);

					errorAbort();

					break;
				}
				else 
				{
					Config::header.frame_num = frame_num;
				}

				Motion::setHeader(&Config::header);

				#if _DEBUG_INSTALL
					char name[22];
					name[20] = '|';
					name[21] = '\0';
					memcpy(name, Config::header.name, 20);

					System::output_serial->print(F("name : "));
					System::output_serial->println(name);
					System::output_serial->print(F("extra[0] : "));
					System::output_serial->println((int)Config::header.extra[0]);
					System::output_serial->print(F("extra[1] : "));
					System::output_serial->println((int)Config::header.extra[1]);
					System::output_serial->print(F("extra[2] : "));
					System::output_serial->println((int)Config::header.extra[2]);
					System::output_serial->print(F("frame_num : "));
					System::output_serial->println((int)Config::header.frame_num);
					System::output_serial->print(F("int. slot : "));
					System::output_serial->println((int)Config::header.slot);
				#endif

				Config::frame.number = 0;

				// FRAME_ELEMENTの到着数を適切な数値に置換
				Packet::DEFINITIONS[State::FRAME_ELEMENT_INCOMING].count = 25 * Config::header.frame_num; // TASK: 定数定義の置換

				break;
			}

			case State::FRAME_ELEMENT_INCOMING:
			{
				static int frame_count = 0;

				if (frame_count == 0)
				{
					unsigned int transition_delay_msec = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());
					Config::frame.transition_delay_msec = transition_delay_msec;
				}
				else
				{
					int joint = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());
					Config::frame.joint_angle[frame_count - 1] = joint;
				}

				frame_count++;

				if (frame_count == 25)
				{
					Motion::setFrame(Config::header.slot, &Config::frame);

					#if _DEBUG_INSTALL
						System::output_serial->print(F("transition_delay_msec : "));
						System::output_serial->println(Config::frame.transition_delay_msec);
						for (int joint = 0; joint < Joint::SUM(); joint++)
						{
							System::output_serial->print(F("joint ["));
							System::output_serial->print(joint);
							System::output_serial->print(F("] : "));
							System::output_serial->println(Config::frame.joint_angle[joint]);
						}
						System::output_serial->print(F("number : "));
						System::output_serial->println((int)Config::frame.number);
					#endif

					frame_count = 0;
					Config::frame.number++;
				}

				break;
			}

			case State::JOINT_ID_ELEMENT_INCOMING:
			{
				Config::joint_id = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());

				if (Config::joint_id > Joint::SUM())
				{
					Config::joint_id = Joint::SUM();
				}

				#if _DEBUG
					System::output_serial->print(F(">>> joint id : "));
					System::output_serial->println((int)Config::joint_id);
				#endif

				break;
			}

			case State::ANGLE_ELEMENT_INCOMING:
			{
				Config::angle = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());

				switch (Token::initial)
				{
					case Token::JOINT_MIN_SETTING_COMMAND:
					{
						Joint::setMinAngle(Config::joint_id, Config::angle);

						break;
					}

					case Token::JOINT_MAX_SETTING_COMMAND:
					{
						Joint::setMaxAngle(Config::joint_id, Config::angle);

						break;
					}

					case Token::JOINT_HOME_SETTING_COMMAND:
					{
						Joint::setHomeAngle(Config::joint_id, Config::angle);

						break;
					}

					case Token::JOINT_MOVE_COMMAND:
					{
						Joint::setAngle(Config::joint_id, Config::angle);

						break;
					}

					default:
					{
						break;
					}
				}

				#if _DEBUG
					System::output_serial->print(F(">>> angle : "));
					System::output_serial->println(Config::angle);
				#endif

				break;
			}

			case State::MOTION_SLOT_ELEMENT_INCOMING:
			{
				unsigned char motion_slot = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());

				if (motion_slot > Motion::SLOT_MAX())
				{
					motion_slot = Motion::SLOT_MAX();
				}

				#if _DEBUG
					System::output_serial->print(F(">>> motion num : "));
					System::output_serial->println(motion_slot);
				#endif

				switch (Token::initial)
				{
					case Token::DUMP_MOTION_COMMAND:
					{
						Config::dumpMotion(motion_slot);

						break;
					}

					case Token::MOTION_PLAY_COMMAND:
					{
						if (!Motion::playing())
						{
							Utility::motionPlayHelper();
							Motion::play(motion_slot);
						}

						break;
					}

					default:
					{
						break;
					}
				}

				break;
			}

			case State::CODE_ELEMENT_INCOMING:
			{
				if (Code::running())
				{
					return;
				}

				static unsigned char call_count = 0;
				static unsigned char motion_slot;
				static unsigned char loop_count;

				if (call_count == 0)
				{
					motion_slot = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());

					if (motion_slot > Motion::SLOT_MAX())
					{
						motion_slot = Motion::SLOT_MAX();
					}

					#ifdef _DEBUG
						System::output_serial->print(F(">>> motion slot : "));
						System::output_serial->println((int)motion_slot);
					#endif

					call_count++;
				}
				else if (call_count == 1)
				{
					loop_count = strtol(Buffer::data, 0, Buffer::TO_NUM__BASE());

					#ifdef _DEBUG
						System::output_serial->print(F(">>> loop count : "));
						System::output_serial->println((int)loop_count);
					#endif

					Code::setCode(motion_slot, loop_count);
					call_count = 0;
				}

				break;
			}

			default:
			{
				#if _DEBUG
					System::output_serial->print(F(">>> token now : "));
					System::output_serial->println(Token::STR[Token::now]);
				#endif

				break;
			}
		}
	}

	// 遷移テーブルを参照して状態遷移する関数です。
	// 内部を編集する必要は基本的にありません。
	void transition()
	{
		#if _DEBUG
			System::output_serial->println(F("in fuction : Purser::transition()"));
			System::output_serial->print(F(">>> state now : "));
			System::output_serial->println(State::STR[State::now]);
		#endif

		if (Token::now == Token::PURSER_ERROR)
		{
			init();

			return;
		}

		Buffer::position = 0;

		if (--Packet::DEFINITIONS[State::now].count != 0)
		{
			#if _DEBUG
				System::output_serial->print(F(">>> packet count : "));
				System::output_serial->println(Packet::DEFINITIONS[State::now].count);
			#endif

			return;
		}

		State::now = State::TRANSITION[State::now][Token::now];

		if (State::now == State::INIT)
		{
			init();
		}
	}
}


void setup()
{
	PLEN2::setup();
}


void loop()
{
	PLEN2::loop();
}
