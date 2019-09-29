#include <stoked/btp/torrent.h>
#include <stoked/btp/bencode.h>
#include <stoked/btp/sha1.h>

namespace stoked::btp {

    //std::shared_ptr<torrent> torrent::load_torrent_file(const std::string& filename) {
    //    auto bencode_doc = bencode_doc::parse_doc(filename);
    //    if (!bencode_doc) {
    //        return nullptr;
    //    }

    //    auto reenc = bencode_doc->encoded_val();

    //    if (bencode_doc->state() == bencode_parse_state::parsed) {
    //        auto main_dict = bencode_doc.get()->get_element(0).dict_val();
    //        if (auto info_entry = main_dict["info"]; info_entry != nullptr) {
    //            auto info_dict = info_entry->dict_val();
    //            std::string pieces = info_dict["pieces"]->str_val();
    //            size_t piecelen = info_dict["piece length"]->int_val();
    //            std::string name = info_dict["name"]->str_val();

    //            bool single_file = false;
    //            std::vector<torrent_file> torrent_files{ };
    //            if (auto files_entry = info_dict.at("files"); files_entry != nullptr) {

    //                auto file_list = files_entry->list_val();

    //                for (const auto& entry : file_list) {
    //                    auto file_dict = entry->dict_val();
    //                    auto path_list = file_dict["path"]->list_val();

    //                    std::vector<std::string> paths{path_list.size()};
    //                    for (const auto& path_entry : path_list) {
    //                        paths.push_back(path_entry->str_val());
    //                    }

    //                    torrent_files.emplace_back(
    //                        /*std::move(torrent_file {*/paths, size_t(file_dict["length"]->int_val())/*})*/
    //                    );
    //                }
    //            } else {
    //                single_file = true;

    //                torrent_files.emplace_back(
    //                    /*std::move(torrent_file {*/std::vector<std::string>{name}, size_t(info_dict["length"]->int_val())/*})*/
    //                );
    //            }

    //            std::string comment = main_dict.find("comment") != main_dict.cend() ? main_dict["comment"]->str_val() : "";
    //            std::string created_by = main_dict.find("created by") != main_dict.cend() ? main_dict["created by"]->str_val() : "";
    //            intptr_t creation = main_dict.find("creation date") != main_dict.cend() ? main_dict["creation date"]->int_val() : 0;

    //            auto announces = std::vector<std::string>{
    //                main_dict["announce"]->str_val()
    //            };

    //            for (const auto& announce : main_dict["announce-list"]->list_val()) {
    //                if (announce) {
    //                    for (const auto& item : announce->list_val()) {
    //                        announces.push_back(item->str_val());
    //                    }
    //                }
    //            }

    //            utils::sha1 checksum;
    //            checksum.update(info_entry->encoded_val());
    //            std::string hash_str = checksum.final();

    //            // uint32_t ints[5] = checksum.raw_digest();/*
    //            // checksum.raw(ints);*/*/

    //            std::vector<char> bytes;
    //            for (size_t i = 0; i < 5; i++) {
    //                bytes.push_back(checksum.raw_digest()[i] >> 24 & 0xFF);
    //                bytes.push_back(checksum.raw_digest()[i] >> 16 & 0xFF);
    //                bytes.push_back(checksum.raw_digest()[i] >> 8 & 0xFF);
    //                bytes.push_back(checksum.raw_digest()[i] & 0xFF);
    //            }

    //            return std::make_shared<torrent>(
    //                filename,
    //                name,
    //                torrent_files,
    //                pieces,
    //                piecelen,
    //                comment,
    //                created_by,
    //                creation,
    //                announces,
    //                single_file,
    //                hash_str,
    //                bytes
    //                );
    //        }
    //    }

    //    return (nullptr);
    //}

    std::shared_ptr<torrent> torrent::load_torrent_file(const std::string& filename)
    {
        auto bencode_doc = bencode_doc::parse_doc(filename);
        if (!bencode_doc)
        {
            return nullptr;
        }

        auto reenc = bencode_doc->encoded_val();

        if (bencode_doc->state() == bencode_parse_state::parsed)
        {
            auto main_dict = bencode_doc.get()->get_element(0).dict_val();
            if (auto info_entry = main_dict["info"]; info_entry != nullptr)
            {
                auto info_dict = info_entry->dict_val();
                std::string pieces = info_dict["pieces"]->str_val();
                size_t piecelen = info_dict["piece length"]->int_val();
                std::string name = info_dict["name"]->str_val();

                bool single_file = false;
                std::vector<torrent_file> torrent_files{ };
                if (auto files_entry = info_dict.at("files"); files_entry != nullptr)
                {

                    auto file_list = files_entry->list_val();

                    for (const auto& entry : file_list)
                    {
                        auto file_dict = entry->dict_val();
                        auto path_list = file_dict["path"]->list_val();

                        std::vector<std::string> paths{path_list.size()};
                        for (const auto& path_entry : path_list)
                        {
                            paths.push_back(path_entry->str_val());
                        }

                        torrent_files.emplace_back(
                            /*std::move(torrent_file {*/paths, size_t(file_dict["length"]->int_val())/*})*/
                        );
                    }
                }
                else
                {
                    single_file = true;

                    torrent_files.emplace_back(
                        /*std::move(torrent_file {*/std::vector<std::string>{name}, size_t(info_dict["length"]->int_val())/*})*/
                    );
                }

                std::string comment = main_dict.find("comment") != main_dict.cend() ? main_dict["comment"]->str_val() : "";
                std::string created_by = main_dict.find("created by") != main_dict.cend() ? main_dict["created by"]->str_val() : "";
                intptr_t creation = main_dict.find("creation date") != main_dict.cend() ? main_dict["creation date"]->int_val() : 0;

                auto announces = std::vector<std::string>{
                    main_dict["announce"]->str_val()
                };

                for (const auto& announce : main_dict["announce-list"]->list_val())
                {
                    if (announce)
                    {
                        for (const auto& item : announce->list_val())
                        {
                            announces.push_back(item->str_val());
                        }
                    }
                }

                return std::make_shared<torrent>(
                    filename,
                    name,
                    torrent_files,
                    pieces,
                    piecelen,
                    comment,
                    created_by,
                    creation,
                    announces,
                    single_file,
                    info_hash(info_entry->encoded_val())
                    );
            }
        }

        return (nullptr);
    }
}