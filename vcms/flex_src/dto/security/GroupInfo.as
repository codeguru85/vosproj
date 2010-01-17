package dto.security
{
	import mx.collections.ArrayCollection;
	
	[Managed]
	[RemoteClass(alias="com.broadengate.security.model.GroupInfo")]
	public class GroupInfo
	{
		public var groupId:int;
 
		public var groupName:String;
	
		public var createTime:Date;
		
		public var groupInfo:GroupInfo;

		public var userInfos:ArrayCollection; 

		public var groupInfos:ArrayCollection;
		
		public function GroupInfo() 
		{
			//TODO: implement function
		}

	}
}