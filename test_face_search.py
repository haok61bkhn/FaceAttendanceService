import grpc
import sys
from functools import partial
import gen_py.featuresmanager_pb2 as featuresmanager_message
import gen_py.featuresmanager_pb2_grpc as featuresmanager_service


class ServiceClient:

    def __init__(self, service_module, stub_name, host, port, timeout=10):
        channel = grpc.insecure_channel("{0}:{1}".format(host, port))
        try:
            grpc.channel_ready_future(channel).result(timeout=10)
        except grpc.FutureTimeoutError:
            sys.exit("Error connecting to server")
        self.stub = getattr(service_module, stub_name)(channel)
        self.timeout = timeout

    def __getattr__(self, attr):
        return partial(self._wrapped_call, self.stub, attr)

    # args[0]: stub, args[1]: function to call, args[3]: Request
    # kwargs: keyword arguments
    def _wrapped_call(self, *args, **kwargs):
        try:
            return getattr(args[0], args[1])(args[2], **kwargs, timeout=self.timeout)
        except grpc.RpcError as e:
            print("Call {0} failed with {1}".format(args[1], e.code()))
            raise


if __name__ == "__main__":
    featuresmng = ServiceClient(
        featuresmanager_service, "FeaturesManagerStub", "localhost", 50052
    )

    ## Init FeatureGroup
    group_id = "group_test"
    group_name = "Danh sách học viên"

    response = featuresmng.InitFeatureGroup(
        featuresmanager_message.GroupSet(group_id=group_id, group_name=group_name)
    )
    print("InitFeatureGroup:", response.status)

    ## Get GetGroupList
    response = featuresmng.GetGroupList(featuresmanager_message.Empty())
    for group in response.groups:
        print(group.group_id, group.group_name)

    # ## Remove FeatureGroup
    # response = featuresmng.RemoveFeatureGroup(
    #     featuresmanager_message.GroupSet(group_id=group_id)
    # )
    # print("RemoveFeatureGroup:", response.status)

    ## Insert Feature
    features = [0.1 for i in range(512 * 2)]
    response = featuresmng.InsertFeature(
        featuresmanager_message.FeatureSet(
            group_id=group_id, feature_id="hao2", features=features
        )
    )
    print("InsertFeature:", response.status)

    # # ========================remove=================================
    # response = featuresmng.RemoveFeatureID(
    #     featuresmanager_message.FeatureMeta(feature_id="hao2", group_id=group_id)
    # )
    # if response:
    #     print("Remove created:", response.status)

    # ===========================Search===================================
    features = [0.1 for i in range(512)]
    response = featuresmng.SearchFeature(
        featuresmanager_message.FeatureSearchingSet(
            group_id=group_id, features=features, topk=100, min_score=0.5
        ),
        # metadata=metadata
    )

    if response:
        print("status :", response.ids)
        print("status :", response.scores)
        print("status :", response.distances)
