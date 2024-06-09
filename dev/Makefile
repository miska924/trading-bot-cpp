curr_user=$(shell whoami)
dev_docker_compose="docker-compose.yml"

container_name="trading_bot_${curr_user}"

#====================================================================

help:   ## Show this help.
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

#====================================================================

up:		## Run dev container
	docker-compose -f ${dev_docker_compose} up -d --build

stop:	## Stop dev container
	docker-compose -f ${dev_docker_compose} stop

down:	## Stop and remove dev container
	docker-compose -f ${dev_docker_compose} down

ps:		## Show curren user's running containers
	docker-compose ps

log:	## Show logs of container
	docker logs ${container_name}

bot_bash:	## Attach to trading bot dev container
	docker exec -it ${container_name} bash