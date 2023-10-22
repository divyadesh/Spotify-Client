#include "lib/spotify/api.hpp"
#include "lib/uri.hpp"

lib::spt::api::api(lib::settings &settings, const lib::http_client &http_client,
	lib::spt::request &request)
	: settings(settings),
	http(http_client),
	request(request)
{
}

void lib::spt::api::refresh(bool force)
{
	request.refresh(force);
}

auto lib::spt::api::error_message(const std::string &url, const std::string &data) -> std::string
{
	nlohmann::json json;
	try
	{
		if (!data.empty())
		{
			json = nlohmann::json::parse(data);
		}
	}
	catch (const std::exception &e)
	{
		lib::log::warn("{} failed: {}", url, e.what());
		return {};
	}

	if (json.is_null() || !json.is_object() || !json.contains("error"))
	{
		return {};
	}

	auto message = json.at("error").at("message").get<std::string>();
	if (!message.empty())
	{
		lib::log::error("{} failed: {}", url, message);
	}
	return message;
}

auto lib::spt::api::follow_type_string(lib::follow_type type) -> std::string
{
	switch (type)
	{
		case lib::follow_type::artist:
			return "artist";

		case lib::follow_type::user:
			return "user";
	}

	return {};
}

//region GET

void lib::spt::api::get(const std::string &url, lib::callback<nlohmann::json> &callback)
{
	http.get(lib::spt::to_full_url(url), request.auth_headers(),
		[url, callback](const std::string &response)
		{
			try
			{
				callback(response.empty()
					? nlohmann::json()
					: nlohmann::json::parse(response));
			}
			catch (const nlohmann::json::parse_error &e)
			{
				lib::log::error("{} failed to parse: {}", url, e.what());
				lib::log::debug("JSON: {}", response);
			}
			catch (const std::exception &e)
			{
				lib::log::error("{} failed: {}", url, e.what());
			}
		});
}

void lib::spt::api::get_items(const std::string &url, const std::string &key,
	lib::callback<nlohmann::json> &callback)
{
	constexpr size_t api_prefix_length = 27;

	auto api_url = lib::strings::starts_with(url, "https://api.spotify.com/v1/")
		? url.substr(api_prefix_length)
		: url;

	get(api_url, [this, key, callback](const nlohmann::json &json)
	{
		if (!key.empty() && !json.contains(key))
		{
			lib::log::error(R"(no such key "{}" in "{}")", key, json.dump());
		}

		const auto &content = key.empty() ? json : json.at(key);
		const auto &items = content.at("items");
		if (content.contains("next") && content.at("next").is_string())
		{
			const auto &next = content.at("next").get<std::string>();
			get_items(next, key, [items, callback](const nlohmann::json &next)
			{
				callback(lib::json::combine(items, next));
			});
			return;
		}
		callback(items);
	});
}

void lib::spt::api::get_items(const std::string &url, lib::callback<nlohmann::json> &callback)
{
	get_items(url, std::string(), callback);
}

//endregion

//region PUT

void lib::spt::api::put(const std::string &url, const nlohmann::json &body,
	lib::callback<std::string> &callback)
{
	auto header = request.auth_headers();
	header["Content-Type"] = "application/json";

	auto data = body.is_null()
		? std::string()
		: body.dump();

	http.put(lib::spt::to_full_url(url), data, header,
		[this, url, body, callback](const std::string &response)
		{
			auto error = error_message(url, response);

			const auto noDevice = lib::strings::contains(error, "No active device found");
			const auto invalidDevice = lib::strings::contains(error, "Device not found");

			if (noDevice || invalidDevice)
			{
				if (invalidDevice)
				{
					this->request.set_current_device(std::string());
				}

				devices([this, url, body, error, callback]
					(const std::vector<lib::spt::device> &devices)
				{
					if (devices.empty())
					{
						if (callback)
						{
							callback(error);
						}
					}
					else
					{
						this->request.device_select.get(devices, [this, url, body, callback, error]
							(const lib::spt::device &device)
						{
							if (device.id.empty())
							{
								callback(error);
								return;
							}

							this->set_device(device, [this, url, body, callback, device]
								(const std::string &status)
							{
								if (status.empty())
								{
									this->request.set_current_device(device.id);
									this->put(get_device_url(url, device), body, callback);
								}
							});
						});
					}
				});
			}
			else if (callback)
			{
				callback(error);
			}
		});
}

void lib::spt::api::put(const std::string &url, lib::callback<std::string> &callback)
{
	put(url, nlohmann::json(), callback);
}

//endregion

//region POST

void lib::spt::api::post(const std::string &url, lib::callback<std::string> &callback)
{
	auto headers = request.auth_headers();
	headers["Content-Type"] = "application/x-www-form-urlencoded";

	http.post(lib::spt::to_full_url(url), headers,
		[url, callback](const std::string &response)
		{
			callback(error_message(url, response));
		});
}

void lib::spt::api::post(const std::string &url, const nlohmann::json &json,
	lib::callback<nlohmann::json> &callback)
{
	auto headers = request.auth_headers();
	headers["Content-Type"] = "application/json";

	auto data = json.is_null()
		? std::string()
		: json.dump();

	http.post(lib::spt::to_full_url(url), data, headers,
		[url, callback](const std::string &response)
		{
			try
			{
				callback(response.empty()
					? nlohmann::json()
					: nlohmann::json::parse(response));
			}
			catch (const nlohmann::json::parse_error &e)
			{
				lib::log::error("{} failed to parse: {}", url, e.what());
				lib::log::debug("JSON: {}", response);
			}
			catch (const std::exception &e)
			{
				lib::log::error("{} failed: {}", url, e.what());
			}
		});
}


//endregion

//region DELETE

void lib::spt::api::del(const std::string &url, const nlohmann::json &json,
	lib::callback<std::string> &callback)
{
	auto headers = request.auth_headers();
	headers["Content-Type"] = "application/json";

	auto data = json.is_null()
		? std::string()
		: json.dump();

	http.del(lib::spt::to_full_url(url), data, headers,
		[url, callback](const std::string &response)
		{
			callback(error_message(url, response));
		});
}

void lib::spt::api::del(const std::string &url, lib::callback<std::string> &callback)
{
	del(url, nlohmann::json(), callback);
}

//endregion
