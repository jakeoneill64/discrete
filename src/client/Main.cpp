//
// Created by Jake M O'Neill on 01/08/2022.
//

#include "render/RenderClient.h"
#include "Log.h"
#include <spdlog/spdlog.h>
#include <memory>

int main(){

    logging::configure("client");

    auto logger = spdlog::get("client");

    logger->info("TEST!!!");

    logger->flush();

//    Render::RenderConfiguration config{3, 3, 1, 800, 600, std::string{"JakeyWakey"}, 0};
//    Render::RenderClient client{config};


}

